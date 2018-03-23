#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "rom.h"
#include "opcodes.h"
#include "emu.h"
#include "debug.h"
#include "error.h"
#include "gpu.h"

int main(int argc, char **argv) {
  if (argc == 1) {
    exit(-1);
  }

  emu gb_emu;
  emu *gb_emu_p = &gb_emu;

  cpu *z80_p = &(gb_emu.z80);
  rom *gb_rom_p = &(gb_emu.gb_rom);

  init_emu(gb_emu_p);

  load_rom(gb_rom_p, argv[1]);

  set_PC(z80_p, 0x100);

  opcode op_store;
  int op;
  while (true) {
    //printf("Now executing instruction at address 0x%04x\n", get_PC(z80_p));
    #ifdef DEBUG
    if(debug_prompt(gb_emu_p) == ERR_READ_LINE) {
      exit(0);
	}
    #endif

    op = dispatch_op(gb_emu_p, &op_store);
    if (op == ERR_OP_INVALID_OR_NOT_IMPLEMENTED) {
      printf("Unimplemented op: 0x%02x\n", op_store.op);
      exit(0);
    }
    if (op == ERR_INVALID_ADDRESS) {
      printf("Invalid address in op\n");
      exit(0);
    }
    //step_gpu(gb_emu_p);
    //printf("Successfully dispatched op: %02x\n", op);
  }
}
