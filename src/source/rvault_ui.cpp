#include <iostream>
#include "../headers/rvault_ui.h"

#include <cstring>
#include <termios.h>
#include <unistd.h>
#include "../headers/rvault_platform.h"
#include "../headers/rvault_constants.h"
#include "../headers/rvault_exception.h"
#include "../headers/rvault_session.h"


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
        }catch (GenericException e) {
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
 * 6 - Change Master Password
 * 7 - Exit
 */
int mainMenu() {

    std::cout << "RVault\n";
    std::cout << "Options:\n";
    std::cout << "1 - List Entries\n"
                 "2 - Add Entry\n"
                 "3 - Remove Entry\n"
                 "4 - Show Entry Password\n"
                 "5 - Delete Vault\n"
                 "6 - Change Master Password\n"
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

    memset(&newEntry, 0, sizeof(RVaultEntryPlain));
    session.encryptEntry(plainEntry, &newEntry);
    session.addEntry(&newEntry);

    std::cout << "Entry Added!\n";
    press_enter_to_continue();
}



