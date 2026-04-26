#ifdef __cplusplus
extern "C" {
#endif

#ifndef RVAULT_RVAULT_RANDOM_H
#define RVAULT_RVAULT_RANDOM_H
#include <stddef.h>
#include <stdint.h>


/*
 * Requests random bytes from the operating system
 */
int rvault_random_bytes(uint8_t *out, size_t len);



/*
 * Generates random ascii characters from the libsodium api
 *
 * Buffer must be at least len + 1 bytes.
 * Return Codes:
 * 0: Success
 * -1: generation error
 */
int rvault_random_chars(char *chars, size_t len);

#endif //RVAULT_RVAULT_RANDOM_H

#ifdef __cplusplus
    }
#endif