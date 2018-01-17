#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

int byte_to_2c(uint8_t val);

void get_user_input(char *buf);

#define MAX_USER_INPUT_LENGTH 100
#endif /* UTIL_H */
