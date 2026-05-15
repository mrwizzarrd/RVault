#include "../headers/rvault_platform.h"
#include <string.h>
#include "../headers/rvault_constants.h"

#if defined(RVAULT_PLATFORM_WINDOWS)
#include <windows.h>
#include <lmcons.h>

#elif defined(RVAULT_PLATFORM_LINUX) || defined(RVAULT_PLATFORM_UNIX) || defined(RVAULT_PLATFORM_MACOS)
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>

#endif

/*
 *platform codes
 * 1229: Windows
 * 1256: Linux
 * 1345: MACOS
 * 1350: Unix
 * -1: error
 */
int rvault_get_platform() {
#if defined(_WIN64) || defined(_WIN32)
#define PLATFORM WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#define PLATFORM MACOS
#elif defined(__linux__)
#define PLATFORM LINUX
#elif defined(__unix__)
#define PLATFORM UNIX
#else
#define PLATFORM -1
#endif
    const int platform = PLATFORM;
    return platform;
}


/*
 *
 * return codes
 * -1: generic error
 * -2: incorrect out buffer size
 * 0: success
 *
 */

int rvault_get_username(char *out) {

#if defined(RVAULT_PLATFORM_WINDOWS)
    if (strlen(out) != UNLEN + 1) {
        return -2;
    }
    unsigned long out_len = UNLEN + 1;
    wchar_t username[out_len];
    if (!GetUserNameW(username, &out_len)) {
        return -1;
    }
    wcstombs(out, username, out_len);
    return 0;

#else
    long name_max = sysconf(LOGIN_NAME_MAX);
    if (name_max <= 0) {
        name_max = 256;
    }

    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (!pw || !pw->pw_name) {
        return -1;
    }

    if (strlen(pw->pw_name) >= (size_t) name_max) {
        return -2;
    }

    strcpy(out, pw->pw_name);

    return 0;

#endif
}

/*
 * Windows: %LOCALAPPDATA%\RVault
 * Mac: ~/Library/Application Support/rvault
 * Linux: ~/.local/share/rvault
 */
int rvault_get_path(char *out, size_t len) {
    if (len != MAX_PATH) {
        return -1;
    }
#if defined(RVAULT_PLATFORM_LINUX)
    char* home = getenv("HOME");
    if (!home) {
        return -2; //TODO later create getpwuid fallback
     }
    size_t home_size = strlen(home);
    if (len < home_size + 21) return -2;
    for (int i = 0; i < home_size; i++) {
        out[i] = home[i];
    }
    char rest_of_path[21] = "/.local/share/rvault/";
    for (int i = home_size; i < home_size + 21; i++) {
        out[i] = rest_of_path[i - home_size];
    }
#elif defined(RVAULT_PLATFORM_MACOS)
#error "No MACOS Support Yet"
#elif defined(RVAULT_PLATFORM_WINDOWS)
    char* local_app_data = getenv("LOCALAPPDATA");
    if (!local_app_data) {
        return -2; //TODO later create GetUserProfileDirectory fallback from the win32 API
    }
    size_t appdata_size = strlen(local_app_data);

    if (len < appdata_size + 9) return -2;
    for (int i = 0; i < home_size; i++) {
        out[i] = home[i];
    }
    char rest_of_path[9] = "\\rvault\\";
    for (int i = home_size; i < home_size + 9; i++) {
        out[i] = rest_of_path[i - home_size];
    }
#endif

    return 0;
}
