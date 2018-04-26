#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

#define DD_MASK 0b00110000
#define SS_MASK 0b00110000
#define QQ_MASK 0b00110000
#define REG_MASK 0b00000111

// Given an opcode, returns a dd code (bits 4 and 5)
uint8_t get_dd_code(uint8_t op);

// Given an opcode, returns an ss code (bits 4 and 5)
uint8_t get_ss_code(uint8_t op);

// Given an opcode, returns the bit offset (bits 3-5)
uint8_t get_bit_offset(uint8_t op);

// Given an opcode, returns a qq code (bits 4 and 5)
uint8_t get_qq_code(uint8_t op);

// Given an opcode, returns a reg code (bits 0-2)
uint8_t get_reg_code_lo(uint8_t op);

// Given an opcode, returns a reg code (bits 3-5)
uint8_t get_reg_code_hi(uint8_t op);

// Given an opcode, returns a t code (bits 3-5)
uint8_t get_t_code(uint8_t op);

// Gets the two's complement interpretation of a byte
int byte_to_2c(uint8_t val);

// Fills buf with a line of user input. Returns 0 on success,
// or ERR_READ_LINE on failure
int get_user_input(char *buf, int size);

// Max of two ints
int imax(int a, int b);

// Min of two unsigned ints
int imin(int a, int b);

// Given an N x M array of uint8_t's, returns a resized array of N_1 x N_2
int resize_array(uint16_t old_w, uint16_t old_h, uint16_t new_w, uint16_t new_h, uint8_t array[old_h][old_w], uint8_t resized_array[new_h][new_w], bool flip);


#endif /* UTIL_H */
