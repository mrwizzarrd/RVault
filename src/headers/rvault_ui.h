#ifndef RVAULT_RVAULT_UI_H
#define RVAULT_RVAULT_UI_H

#include <iostream>
#include "rvault_crypto.h"
#include "rvault_vault.h"
#include "rvault_session.h"

//Helper Function for Converting bytes to text
std::string bytes_to_string(const uint8_t* bytes);


//CLI Pasword Getter
std::string get_master_password();


//clears terminal
void clear();


//First Time Setup For User
void firstTimeSetup(std::string& nameOut, std::string& masterOut);


/*
 * Prompt for adding entry
 *
 * Enter Entry Name:
 * Enter Entry Username:
 * Enter Entry Password:
 *
 * Name: example.com
 * Username: Example
 * Password: *******
 * Is This Correct [Y/N]?
 */
void addNewEntry();

/*
 * Display Style
 *
 * Entries
 * ---------
 * Entry 1.) ExampleName1
 * Entry 2.) ExampleName2
 * ...
 * Entry N.) ExampleNameN
 */
void displayEntries(RVaultSession& session);

/*
 * Selections:
 * 1 - List Entries
 * 2 - Add Entry
 * 3 - Remove Entry
 * 4 - Show Entry Password
 * 5 - Delete Vault
 * 6 - Generate Password
 * 7 - Exit
 *
 *  Returns selection as an int
 */
int mainMenu();


/*
 * Uses the addEntry method from the RVaultSession Class
 */
void addNewEntry(RVaultSession& session);

/*
 * Removes entry from session
 */
void rmEntry(RVaultSession& session);


/*
 * Displays user entry after locating it in the entry's list and authenticating master password
 */
void showEntry(RVaultSession& session);


#endif //RVAULT_RVAULT_UI_H