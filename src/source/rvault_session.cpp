#include "../headers/rvault_constants.h"
#include "../headers/rvault_vault.h"
#include "../headers/rvault_file.h"
#include "../headers/rvault_session.h"
#include "../headers/rvault_ui.h"
#include "../headers/rvault_platform.h"

#include <filesystem>
#include <cstring>
#include "../headers/rvault_exception.h"
#include <ctime>

namespace fs = std::filesystem;

RVaultSession::RVaultSession(std::string& master_password) {
    char path_char[MAX_PATH];
    rvault_get_path(path_char, MAX_PATH);
    std::string path = path_char;

    fs::path test_path = path;

    if (!fs::exists(test_path)) {
        fs::create_directories(test_path);
    }

    pth = path + "vault.rvault";
    std::time_t login_time;
    RVaultFile vault_file;
    bool open;
    if (!fs::exists(pth)) {
        std::cout << "No User Detected, Initiating First Time Setup\n";

        std::string userName;
        firstTimeSetup(userName, master_password);
        strncpy(reinterpret_cast<char *>(header.owner), userName.c_str(), VAULT_OWNER_NAME_MAX_LEN - 1);

        vault_file.create(pth, master_password.c_str(), this->header);
    } else {
        master_password = get_master_password();
        open = vault_file.open(pth, master_password.c_str(), &this->entries, this->header);
        if (!open) {
            std::cout << "Error rvault_session.cpp line 31\n";
            throw GenericException("Failed to Open File");
        }
        std::cout << "Welcome Back, " << bytes_to_string(header.owner) << "!\n";
    }

    rvault_derive_key(master_password.c_str(), header.salt, key, KEY_SIZE);
    login_time = std::time(nullptr);
    this->header.login_time = login_time;
}

RVaultSession::~RVaultSession() {

    if (fs::exists(pth)) {
        RVaultFile file;

        file.save(this, pth);
    }
    for (auto & entry : entries) {
        sodium_memzero(&entry, sizeof(RVaultEntryEncrypted));
    }
    sodium_memzero(entries.data(), entries.size() * sizeof(RVaultEntryEncrypted));
}

bool RVaultSession::encryptEntry(RVaultEntryPlain entry, RVaultEntryEncrypted* out) const { //TODO: REFACTOR THIS TO MAKE IT LESS COMPLICATED
    uint8_t name_ciphertext[MAX_NAME_LEN + CIPHER_SIZE];
    uint8_t name_nonce[NONCE_SIZE];
    unsigned long long name_ciphertextLen;
    uint8_t username_ciphertext[MAX_USERNAME_LEN + CIPHER_SIZE];
    uint8_t username_nonce[NONCE_SIZE];
    unsigned long long username_ciphertextLen;
    uint8_t pw_ciphertext[MAX_PASSWORD_LEN + CIPHER_SIZE];
    uint8_t pw_nonce[NONCE_SIZE];
    unsigned long long pw_ciphertextLen;

    int status = rvault_encrypt(entry.entry_name, MAX_NAME_LEN, key, name_ciphertext, &name_ciphertextLen, name_nonce);
    if (status != 0) {
        throw GenericException("Failed To Encrypt Entry");
    }
    status = rvault_encrypt(entry.username, MAX_USERNAME_LEN, key, username_ciphertext, &username_ciphertextLen, username_nonce);
    if (status != 0) {
        throw GenericException("Failed To Encrypt Entry");
    }
    status = rvault_encrypt(entry.password, MAX_PASSWORD_LEN, key, pw_ciphertext, &pw_ciphertextLen, pw_nonce);
    if (status != 0) {
        throw GenericException("Failed To Encrypt Entry");
    }
    out->entry_name_cipher_len = static_cast<unsigned int>(name_ciphertextLen);
    out->username_cipher_len = static_cast<unsigned int>(username_ciphertextLen);
    out->password_cipher_len = static_cast<unsigned int>(pw_ciphertextLen);

    memcpy(out->entry_name_cipher, name_ciphertext, name_ciphertextLen);
    memcpy(out->entry_name_nonce, name_nonce, NONCE_SIZE);
    memcpy(out->username_cipher, username_ciphertext, username_ciphertextLen);
    memcpy(out->username_nonce, username_nonce, NONCE_SIZE);
    memcpy(out->password_cipher, pw_ciphertext, pw_ciphertextLen);
    memcpy(out->password_nonce, pw_nonce, NONCE_SIZE);

    sodium_memzero(&entry, sizeof(RVaultEntryPlain));
    return true;
}

bool RVaultSession::addEntry(RVaultEntryEncrypted* entry) {
    if (!entry) {
        return false;
    }
    entries.emplace_back(*entry);
    return true;
}

bool RVaultSession::decryptEntry(RVaultEntryEncrypted entry, RVaultEntryPlain* out) {
    uint8_t name_plaintext[MAX_NAME_LEN];
    unsigned long long name_plaintextLen = MAX_NAME_LEN;
    uint8_t username_plaintext[MAX_USERNAME_LEN];
    unsigned long long username_plaintextLen = MAX_USERNAME_LEN;
    uint8_t pw_plaintext[MAX_PASSWORD_LEN];
    unsigned long long pw_plaintextLen = MAX_PASSWORD_LEN;

    int status = rvault_decrypt(entry.entry_name_cipher, entry.entry_name_cipher_len, key, name_plaintext, &name_plaintextLen, entry.entry_name_nonce);
    if (status != 0) {
        throw GenericException("Decryption of Entry Name Failed");
    }
    status = rvault_decrypt(entry.username_cipher, entry.username_cipher_len, key, username_plaintext, &username_plaintextLen, entry.username_nonce);
    if (status != 0) {
        throw GenericException("Decryption of Username Failed");
    }
    status = rvault_decrypt(entry.password_cipher, entry.password_cipher_len, key, pw_plaintext, &pw_plaintextLen, entry.password_nonce);
    if (status != 0) {
        throw GenericException("Decryption of Password Failed");
    }
    memset(out, 0, sizeof(RVaultEntryPlain));
    memcpy(out->entry_name, name_plaintext, name_plaintextLen);
    memcpy(out->password, pw_plaintext, pw_plaintextLen);
    memcpy(out->username, username_plaintext, username_plaintextLen);
    sodium_memzero(pw_plaintext, sizeof(pw_plaintext));
    sodium_memzero(username_plaintext, sizeof(username_plaintext));
    sodium_memzero(name_plaintext, sizeof(name_plaintext));
    return true;
}

bool RVaultSession::removeEntry(const std::string& name) {
    for (int i = 0; i < entries.size(); i++) {
        auto* unencrypt = new RVaultEntryPlain;
        try {
            decryptEntry(entries.at(i), unencrypt);
        } catch (GenericException& e) {
            sodium_memzero(unencrypt, sizeof(RVaultEntryPlain));
            delete unencrypt;
            throw;
        }

        std::string entry_name(reinterpret_cast<char*>(unencrypt->entry_name));
        if (name == entry_name) {
            entries.erase(entries.begin() + i);
            delete unencrypt;
            return true;
        }
        sodium_memzero(unencrypt, sizeof(RVaultEntryPlain));
        delete unencrypt;
    }
    return false;
}


std::filesystem::path RVaultSession::getPath() {
    return pth;
}

std::vector<RVaultEntryEncrypted> RVaultSession::getEntries() const {
    return entries;
}

bool RVaultSession::getEntry(std::string& name, RVaultEntryEncrypted* out) {
    for (RVaultEntryEncrypted entry : entries) {
        RVaultEntryPlain plainEntry;
        decryptEntry(entry, &plainEntry);
        if (strcmp(name.c_str(), reinterpret_cast<char *>(plainEntry.entry_name)) == 0) { //entry found
            sodium_memzero(&plainEntry, sizeof(RVaultEntryPlain));
            memcpy(out, &entry, sizeof(RVaultEntryEncrypted));
            return true;
        }
        sodium_memzero(&plainEntry, sizeof(RVaultEntryPlain));
    }
    return false;
}

RVaultHeader RVaultSession::getHeader() const {
    return this->header;
}