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
    RVaultSession* session;
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

    }

}