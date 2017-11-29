#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "mmu.h"
#include "rom.h"

rom gb_rom;

int main(int argc, char **argv) {
  init_cpu();
  if (argc == 1) {
    exit(-1);
  }
  load_rom(&gb_rom, argv[1]);
  print_rom_bytes(&gb_rom, 0, 10);
  
}
