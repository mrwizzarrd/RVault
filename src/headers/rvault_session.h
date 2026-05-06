#ifndef RVAULT_RVAULT_SESSION_H
#define RVAULT_RVAULT_SESSION_H
#include <filesystem>

#include "rvault_constants.h"
#include <vector>
#include "rvault_vault.h"
#include "rvault_file.h"

namespace fs = std::filesystem;
class RVaultSession {
public:

    /*
     * Session Object Constructor for RVault
     * Derives key from master password and loads header and entries into memory
     */
    RVaultSession(std::string& master_password);

    /*
     * When the session ends, all the data stored in memory needs to be zeroed for enhanced security
     */
    ~RVaultSession();

    /*
     * returns the value of a single entry by name
     */
    bool getEntry(std::string& name, RVaultEntryEncrypted* out);

    /*
     * Getter method for the entries vector
     */
    std::vector<RVaultEntryEncrypted> getEntries() const;

    /*
     * Getter method for the vault header
     */
    RVaultHeader getHeader() const;

    /*
     * Getter Method for the path
     */
    std::filesystem::path getPath();

    /*
     * Wraps data in an encrypted entry structure
     */
    bool encryptEntry(RVaultEntryPlain entry, RVaultEntryEncrypted* out) const;

    /*
     * Adds an encrypted entry to the session's entry list
     */
    bool addEntry(RVaultEntryEncrypted* entry);

    /*
     * Decrypts an encrypted entry into plaintext and copies it to a PlaintextEntry object
     */
    bool decryptEntry(RVaultEntryEncrypted entry, RVaultEntryPlain* out);

    /*
     * removes an entry from the list of entries by name
     */
    bool removeEntry(const std::string& name);





private:
    std::vector<RVaultEntryEncrypted> entries;
    RVaultHeader header{};
    uint8_t key[KEY_SIZE]{};
    fs::path pth;
};

#endif