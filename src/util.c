#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"
#include "error.h"

int byte_to_2c(uint8_t val) {
  if ((0x80 & val) != 0) {
    // negative
    val = ~val + 1;
    return -1*(int)val;
  }
  return (int)val;
}

int get_user_input(char *buf, int size) {
 if (!fgets(buf, size, stdin)) {
   return ERR_READ_LINE;
 }
 if ((int)strlen(buf) == 1) {
   return ERR_READ_LINE;
 }
 return 0;
}

uint8_t get_dd_code(uint8_t op) {
  return (op & DD_MASK) >> 4;
}

uint8_t get_qq_code(uint8_t op) {
  return (op & QQ_MASK) >> 4;
}

uint8_t get_ss_code(uint8_t op) {
  return (op & SS_MASK) >> 4;
}

uint8_t get_reg_code_lo(uint8_t op) {
  return (op & REG_MASK);
}

uint8_t get_reg_code_hi(uint8_t op) {
  return ((op >> 3) & REG_MASK);
}

uint8_t get_bit_offset(uint8_t op) {
    return ((op >> 3) & REG_MASK);
}

uint8_t get_t_code(uint8_t op) {
  return ((op >> 3) & REG_MASK);
}

int imax(int a, int b) {
  return a > b ? a : b;
}

int imin(int a, int b) {
  return a < b ? a : b;
}

int resize_array(uint16_t old_w, uint16_t old_h, uint16_t new_w, uint16_t new_h, uint8_t array[old_h][old_w], uint8_t resized_array[new_h][new_w], bool flip) {
  float w_ratio = (float)old_w/(float)new_w;
  float h_ratio = (float)old_h/(float)new_h;
  for (int i = 0; i < new_h; i++) {
    for (int j = 0; j < new_w; j++) {
      int x = i;
      if (flip)
	x = new_h - 1 - i;
      resized_array[x][j] =  array[(uint16_t)floor(h_ratio*i)][(uint16_t)floor(w_ratio*j)];
    }
  }

  return 0;
}

