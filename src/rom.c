#include <stdio.h>
#include <stdlib.h>
#include "rom.h"

unsigned char* load_file(const char *name, long *len_store) {
    FILE *fl = fopen(name, "r");
    fseek(fl, 0, SEEK_END);
    long len = ftell(fl);
    unsigned char *ret = malloc(len);
    fseek(fl, 0, SEEK_SET);
    int val = fread(ret, 1, len, fl);
    if (val < len) {
      exit(1);
    }
    fclose(fl);
    *len_store = len;
    return ret;
}

void load_rom(rom *gb_rom, const char *name) {
  long len = 0;
  unsigned char *file = load_file(name, &len);
  gb_rom->data = file;
  gb_rom->size = len;
}

void print_rom_bytes(rom *gb_rom, uint16_t start, uint16_t len) {
  for (int i = start; i < start + len; i+=1) {
    uint8_t val = gb_rom->data[i];
    printf("0x%04x: 0x%02x\n", i, val);
  }
}

uint8_t get_rom_byte(rom *gb_rom, uint16_t start) {
  return gb_rom->data[start];
}
