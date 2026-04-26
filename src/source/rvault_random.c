#include "../headers/rvault_random.h"

#include <stdint.h>

#include "../headers/rvault_platform.h"
#include <stdbool.h>
#include "../headers/rvault_constants.h"

#if RVAULT_PLATFORM_WINDOWS
#include <windows.h>
#include <bcrypt.h>
#elif RVAULT_PLATFORM_MACOS
#include <stdlib.h>
#elif RVAULT_PLATFORM_LINUX || RVAULT_PLATFORM_UNIX
#include <unistd.h>
#include <sys/random.h>
#include <errno.h>
#include <fcntl.h>
#endif

/*
 * Return Codes:
 * 0: Success
 * -25: Platform Error (If somehow preprocessor failed to terminate compilation)
 * -69: generation error
 */
int rvault_random_bytes(uint8_t *out, size_t len){
        //Generate Byte
    #if defined(RVAULT_PLATFORM_WINDOWS)
        //Windows CSPRNG
        NTSTATUS status = BCryptGenRandom(NULL, out, (ULONG) len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (!BCRYPT_SUCCESS(status)) {
            return -69;
        }
        return 0;

    #elif defined(RVAULT_PLATFORM_MACOS)
        //MacOS CSPRNG
    #elif defined(RVAULT_PLATFORM_LINUX) || defined(RVAULT_PLATFORM_UNIX)
        size_t filled = 0;
        while (filled < len) {
            ssize_t n = getrandom(out + filled, len - filled, 0);
            if (n > 0) {
                filled += (size_t) n;
            } else if (n == -1 && errno == EINTR) {
                continue;
            } else {
                return -69;
            }
        }
    return 0;
    #else
        return -25; // Unsupported platform
    #endif
    return 0;
}

/*
 * Buffer must be at least len + 1 bytes.
 * Return Codes:
 * 0: Success
 * -1: generation error
 */
int rvault_random_chars(char *chars, size_t len) {
    if (!chars || len == 0) {
        return -1;
    }
    for (int i = 0; i < len; i++) {
        chars[i] = (char) randombytes_uniform(94) + 33;
    }
    chars[len] = '\0';
    return 0;
}