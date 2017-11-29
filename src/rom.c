#include <stdio.h>
#include <stdlib.h>
#include "rom.h"

unsigned char* load_file(const char *name, long *len_store) {
    FILE *fl = fopen(name, "r");
    fseek(fl, 0, SEEK_END);
    long len = ftell(fl);
    unsigned char *ret = malloc(len);
    fseek(fl, 0, SEEK_SET);
    fread(ret, 1, len, fl);
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

void print_rom_bytes(rom *gb_rom, uint32_t start, uint32_t len) {
  for (int i = start*2; i < len*2; i+=2) {
    uint16_t val = (uint16_t)(gb_rom->data[i] << 8) | gb_rom->data[i+1];
    printf("0x%04x: 0x%04x\n", i/2, val);
  }
}
