#include <iostream>
#include "../headers/rvault_ui.h"

#include <cstring>
#include <termios.h>
#include <unistd.h>
#include <string>
#include <algorithm>

#include "../headers/rvault_auth.h"
#include "../headers/rvault_random.h"
#include "../headers/rvault_platform.h"
#include "../headers/rvault_constants.h"
#include "../headers/rvault_exception.h"
#include "../headers/rvault_session.h"
#include "../../external_lib/clip/clip.h"


std::string bytes_to_string(const uint8_t* bytes) {
    std::string final(reinterpret_cast<const char*>(bytes));
    return final;
}

void clear() {
    std::cout << "\033[2J\033[1;1H";
}

std::string prompt(const std::string& prompt) {
    std::string response;
    std::cout << prompt;
    std::getline(std::cin, response);
    return response;
}

#if defined(RVAULT_PLATFORM_WINDOWS)
std::string quietPrompt() {
    return "";
}
#else
std::string quietPrompt(const std::string& prompt) {
    std::cout << prompt << std::flush;
    termios old_t{}, new_t{};
    tcgetattr(STDIN_FILENO, &old_t);
    new_t = old_t;
    new_t.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);

    std::string response;
    std::getline(std::cin, response);

    tcsetattr(STDIN_FILENO, TCSANOW, &old_t);
    std::cout << "\n";
    return response;
}
#endif


std::string get_master_password() {
    return quietPrompt("Enter Master Password: ");
}

void press_enter_to_continue() {
    std::cout << "Press Enter To Continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


void firstTimeSetup(std::string& nameOut, std::string& masterOut) {
    nameOut = prompt("Name: ");
    masterOut = get_master_password();
}

void displayEntries(RVaultSession& session) {
    if (session.getEntries().empty()) {
        return;
    }
    int i = 1;
    for (RVaultEntryEncrypted entry : session.getEntries()) {
        RVaultEntryPlain plaintext_entry;
        try {
            session.decryptEntry(entry, &plaintext_entry);
        } catch (GenericException& e) {
            std::cout << e.what();
        }

        std::cout << "Entry Name: " << plaintext_entry.entry_name << " | Entry Username: " << plaintext_entry.username <<"\n";
        sodium_memzero(&plaintext_entry, sizeof(RVaultEntryPlain));
    }
    press_enter_to_continue();
}

/*
 * Selections:
 * 1 - List Entries
 * 2 - Add Entry
 * 3 - Remove Entry
 * 5 - Delete Vault
 * 6 - Generate Password
 * 7 - Exit
 */
int mainMenu() {

    std::cout << "RVault\n";
    std::cout << "Options:\n";
    std::cout << "1 - List Entries\n"
                 "2 - Add Entry\n"
                 "3 - Remove Entry\n"
                 "4 - Show Entry\n"
                 "5 - Delete Vault\n"
                 "6 - Generate Password\n"
                 "7 - Exit\n"
                 "Input >";

    int choice;
    std::string line;
    std::getline(std::cin, line);

    try {
        choice = std::stoi(line);
    } catch (...) {
        return -1;
    }
    return choice;
}

void addNewEntry(RVaultSession& session) {
    RVaultEntryEncrypted newEntry;
    RVaultEntryPlain plainEntry;
    std::string name, username, password, password_confirm;
    name = prompt("Enter Name of Entry: ");
    username = prompt("Enter Username: ");

    while (true) {
        password = quietPrompt("Enter Password: ");
        password_confirm = quietPrompt("Re enter Password: ");

        if (password != password_confirm) {
            std::cout << "Passwords do not match, try again\n";
            continue;
        }
        break;
    }

    memset(&plainEntry, 0, sizeof(RVaultEntryPlain));
    memcpy(plainEntry.entry_name, name.c_str(), name.length());
    memcpy(plainEntry.username, username.c_str(), username.length());
    memcpy(plainEntry.password, password.c_str(), password.length());

    memset(&newEntry, 0, sizeof(RVaultEntryEncrypted));
    session.encryptEntry(plainEntry, &newEntry);
    session.addEntry(&newEntry);

    std::cout << "Entry Added!\n";
    press_enter_to_continue();
}

void rmEntry(RVaultSession& session) {
    std::string entryname = prompt("Enter Name of Entry to Remove: ");
    if (session.removeEntry(entryname)) {
        std::cout << "Entry Removed\n";
    } else {
        std::cout << "Entry Not Found\n";
    }
    press_enter_to_continue();
}

void showEntry(RVaultSession& session) {
    std::string entryname = prompt("Enter Name of Entry to Display: ");
    RVaultEntryEncrypted entry;
    if (session.getEntry(entryname, &entry)) {
        int attempts = 0;
        while (attempts < 5) {
            std::string masterpass = get_master_password();
            attempts++;
            uint8_t key[KEY_SIZE];
            rvault_derive_key(masterpass.c_str(), session.getHeader().salt, key, KEY_SIZE);
            if (rvault_authenticate(session.getHeader(), key) == 0) {
                break;
            }
            std::cout << "Invalid Password! (" << 5 - attempts << " attempts left)\n";
            if (attempts == 5) {
                press_enter_to_continue();
                return;
            }
            //Future TODO: Make a 5 min timer after 5 failed attempts
        }
        RVaultEntryPlain decrypted;
        session.decryptEntry(entry, &decrypted);
        std::cout << "Entry Name: " << decrypted.entry_name << "\nEntry Username: " << decrypted.username << "\nEntry Password: " << decrypted.password << "\n";
        sodium_memzero(&decrypted, sizeof(RVaultEntryPlain));

    } else {
        std::cout << "Entry Not Found\n";
    }
    press_enter_to_continue();
}

void generatePassword(RVaultSession& session) {
    int length = 0;
    while (true) {
        try {
            length = std::stoi(prompt("Enter length of password: "));
            break;
        } catch (const std::exception& e) {
            std::cout << "Exception caught: " << e.what() << "\nTry again\n";
        }
    }
    std::string password(length+1, '\0');
    int status = rvault_random_chars(password.data(), length);
    password.resize(length);
    if (status != 0) {
        throw GenericException("Failed to Generate Password");
    }
    std::cout << "Password: " << password << "\n";

    std::string copy = prompt("Copy password to clipboard? [Y/N] (Anything else will be treated as N): ");
    if (copy == "y" || copy == "Y") {
        clip::set_text(password);
    }

    std::string newEntry = prompt("Create entry with password? [Y/N] (Anything else will be treated as N): ");
    if (copy == "y" || copy == "Y") {
        RVaultEntryEncrypted Entry;
        RVaultEntryPlain plainEntry;
        std::string name, username;
        name = prompt("Enter Name of Entry: ");
        username = prompt("Enter Username: ");

        memset(&plainEntry, 0, sizeof(RVaultEntryPlain));
        memcpy(plainEntry.entry_name, name.c_str(), name.length());
        memcpy(plainEntry.username, username.c_str(), username.length());
        memcpy(plainEntry.password, password.c_str(), password.length());

        memset(&Entry, 0, sizeof(RVaultEntryEncrypted));
        session.encryptEntry(plainEntry, &Entry);
        session.addEntry(&Entry);

        sodium_memzero(&plainEntry, sizeof(RVaultEntryPlain));
        sodium_memzero(&Entry, sizeof(RVaultEntryEncrypted));
    }
    std::cout << "Entry Added!\n";
    press_enter_to_continue();

    sodium_memzero(&password, password.size() + 1);
}

void deleteVault(RVaultSession& session) {
    std::string conf = prompt("Are you sure you want to delete the vault [Y/N]? ");
    if (tolower(conf[0]) != 'y') {
        press_enter_to_continue();
        return;
    }

    int attempts = 0;
    while (attempts < 5) {
        std::string masterpass = get_master_password();
        attempts++;
        uint8_t key[KEY_SIZE];
        rvault_derive_key(masterpass.c_str(), session.getHeader().salt, key, KEY_SIZE);
        if (rvault_authenticate(session.getHeader(), key) == 0) {
            break;
        }
        std::cout << "Invalid Password! (" << 5 - attempts << " attempts left)\n";
        if (attempts == 5) {
            press_enter_to_continue();
            return;
        }
    }
    RVaultFile file;
    try {
        file.deleteFile(session.getPath());
        std::cout << "Deleting " << session.getPath() << std::endl;
    } catch (const GenericException& e) {
        std::cout << e.what();
        press_enter_to_continue();
    }
    press_enter_to_continue();
}



