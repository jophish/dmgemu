#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
