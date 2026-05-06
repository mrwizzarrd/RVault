/*
 * File Structure:
 *
 * STRUCT
 * Header{
 *      salt                    (uint8_t)
 *      security phrase nonce   (uint8_t)
 *      security phrase         (uint8_t)
 *      entry count             (uint32_t)
 *     }
 *
 *     STRUCT
 *     EntryN{
 *           name_nonce              (uint8_t)
 *           name_cipher_len         (uint32_t)
 *           name_cipher             (uint8_t)
 *           username_nonce          (uint8_t)
 *           username_cipher_len     (uint32_t)
 *           username_cipher         (uint8_t)
 *           password_nonce          (uint8_t)
 *           password_cipher_len     (uint32_t)
 *           password_cipher         (uint8_t)
 *           }
 */

#ifndef RVAULT_RVAULT_FILE_H
#define RVAULT_RVAULT_FILE_H

class RVaultSession;

#include "../headers/rvault_constants.h"
#include "../headers/rvault_vault.h"
#include <iostream>
#include <fstream>
#include <optional>

#include "rvault_session.h"


/*
 *
 */

class RVaultFile {
public:

    /*
     * Arguments:
     *     const std::string &path :
     *          Filepath to vault file
     *
     *     const char *master_pword :
     *          Master password for vault
     *     RVaultHeader header:
     *          The head of a rvault file
     * Process:
     * Create salt
     * Derive key from master password
     * Create random bytes for authentication phrase
     * Encrypt auth phrase and generate nonce
     *
     */
    bool create(const std::string& path, const char *master_pword, RVaultHeader& header);

    /*
     * Arguments:
     *     const std::string &path :
     *          Filepath to vault file
     *     const char *master_pword :
     *          Master password for vault
     *     std::vector<RVaultEntry>* entries :
     *          Pointer to vector of vault entries
     *     RVaultHeader header:
     *          The head of a rvault file
     *
     *     Loads file into memory
     *     Writes data to header property
     *     Adds an entry to entries vector for each entry in file
     *
     */
    bool open(const std::string& path, const char *master_pword, std::vector<RVaultEntryEncrypted>* entries, RVaultHeader& header);

    /*
     * Updates the .rvault file with the up to date list of entries, and header
     */
    bool  save(RVaultSession* session, const std::string& filename);

    /*
     * Frees Memory for current File
     */
    void close();

    /*
     * Seleted file, frees memory for file and zeroes the memory
     */
    void deleteFile(const std::filesystem::path& pth);

    //------ Getters

    RVaultHeader getHeader() const;

private:
    std::fstream file;
    RVaultHeader header;
};

#endif //RVAULT_RVAULT_FILE_H