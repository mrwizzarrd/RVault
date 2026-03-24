#ifndef RVAULT_RVAULT_UI_H
#define RVAULT_RVAULT_UI_H

#include <iostream>
#include "rvault_crypto.h"
#include "rvault_vault.h"

//Helper Function for Converting bytes to text
std::string bytes_to_string(const uint8_t* bytes);


//CLI Pasword Getter
std::string get_master_password();


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
void displayEntries();



#endif //RVAULT_RVAULT_UI_H