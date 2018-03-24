#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define DD_MASK 0b00110000
#define REG_MASK 0b00000111

// Given an opcode, returns a dd code (bits 4 and 5)
uint8_t get_dd_code(uint8_t op);

// Given an opcode, returns a reg code (bits 0-2)
uint8_t get_reg_code(uint8_t op);

// Gets the two's complement interpretation of a byte
int byte_to_2c(uint8_t val);

// Fills buf with a line of user input. Returns 0 on success,
// or ERR_READ_LINE on failure
int get_user_input(char *buf, int size);

#endif /* UTIL_H */
