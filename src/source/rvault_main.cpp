#include <filesystem>
#include <iostream>
#include <string>
#include <cstring>
#include "../headers/rvault_platform.h"
#include "../headers/rvault_auth.h"
#include "../headers/rvault_random.h"
#include "../headers/rvault_crypto.h"
#include "../headers/rvault_constants.h"
#include "../headers/rvault_exception.h"
#include "../headers/rvault_file.h"
#include "../headers/rvault_ui.h"
namespace fs = std::filesystem;


int main(void){
    int failed_attempts = 0;
    bool unlocked = false;
    RVaultSession* session = nullptr;
    while (!unlocked && failed_attempts <= 5) {
        try {
            std::string masterPwd;
            session = new RVaultSession(masterPwd);
            unlocked = true;
            break;
        } catch (InvalidPasswordException& e) {
            failed_attempts++;
            std::cout << "invalid password\n";
        }

        if (failed_attempts >= 5) {
            std::cout << "Too many failed attempts, try again later\n";
        }
    }
    if (unlocked) {
        while (true) { //main loop
            clear();
            int main_menu_choice = mainMenu();

            std::cout << "Choice: " << main_menu_choice << std::endl;

            if (main_menu_choice == 7) {
                break;
            }


            switch (main_menu_choice) {
                case 1:
                    displayEntries(*session);
                    continue;
                case 2:
                    addNewEntry(*session);
                    continue;
                case 3:
                    rmEntry(*session);
                    continue;
                case 4:
                    continue;
                case 5:
                    continue;
                case 6:
                    continue;
                default:
                    std::cout << "Invalid Input!\n";
                    break;

            }
        }

    }

    delete session;
    return 0;

}