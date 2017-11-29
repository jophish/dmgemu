#ifndef ROM_H
#define ROM_H

#include <stdint.h>
#include <stdbool.h>

// Just an initial MVP rep
typedef struct rom {
  long size;
  unsigned char *data; // Verbatim contents of ROM
} rom;

// Loads a file into a byte array
unsigned char* load_file(const char *name, long *len_store);

// Given a filename of a ROM, loads it and
// populates a rom struct
void load_rom(rom *gb_rom, const char *name);

void print_rom_bytes(rom *gb_rom, uint32_t start, uint32_t len);

#endif /* ROM_H */
