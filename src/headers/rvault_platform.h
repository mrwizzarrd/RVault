#ifdef __cplusplus
extern "C" {
#endif

#ifndef RVAULT_PLATFORM_H
#define RVAULT_PLATFORM_H
#include "./rvault_constants.h"

#if defined(_WIN64) || defined(_WIN32)
#define RVAULT_PLATFORM_WINDOWS 1 //A.K.A. SPYWARE_OS
#define MAX_PATH MAX_PATH_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#define RVAULT_PLATFORM_MACOS 1
#define MAX_PATH MAX_PATH_MACOS
#elif defined(__linux__)
#define RVAULT_PLATFORM_LINUX 1
#define MAX_PATH MAX_PATH_LINUX
#elif defined(__unix__)
#define RVAULT_PLATFORM_UNIX 1
#define MAX_PATH MAX_PATH_LINUX
#else
#error "Unsupported platform"
#endif




/*
 *
 * return codes
 * -1: generic error
 * -2: incorrect out buffer size
 * 0: success
 *
 */

int rvault_get_username(char *out);
#endif


/*
 * Retrieves the platform that RVault is running on to guarantee compatability
 *platform codes
 * 1229- Windows
 * 1256- Linux
 * 1345- MACOS
 * 1350- Unix
 * -1- error
 */
int rvault_get_platform();

/*
 * Retrieves path as a c
 */
int rvault_get_path(char *out, size_t len);

#ifdef __cplusplus
    }
#endif
