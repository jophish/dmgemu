#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>


int byte_to_2c(uint8_t val);

// Fills buf with a line of user input. Returns 0 on success,
// or ERR_READ_LINE on failure
int get_user_input(char *buf, int size);

#endif /* UTIL_H */
