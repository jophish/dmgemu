#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "mmu.h"
#include "rom.h"
#include "emu.h"
#include "opcodes.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    exit(-1);
  }

  emu gb_emu;
  emu *gb_emu_p = &gb_emu;

  cpu *z80_p = &(gb_emu.z80);

  rom *gb_rom_p = &(gb_emu.gb_rom);

  init_cpu(z80_p);
  load_rom(gb_rom_p, argv[1]);
  set_PC(z80_p, 0x100);

  while (true) {
    //printf("Now executing instruction at address 0x%04x\n", get_PC(z80_p));
    dispatch_op(gb_emu_p);
    //printf("Successfully dispatched op: %02x\n", op);
  }
}
