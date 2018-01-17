#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int byte_to_2c(uint8_t val) {
  if ((0x80 & val) != 0) {
    // negative
    val = ~val + 1;
    return -1*(int)val;
  }
  return (int)val;
}

void get_user_input(char *buf) {
  //char line[MAX_USER_INPUT_LENGTH];
}
