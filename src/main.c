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
#include "window.h"
#include "int.h"

#include <GLFW/glfw3.h>

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
  GLFWwindow *window = init_window(gb_emu_p);
  while (true) {
    //printf("Now executing instruction at address 0x%04x\n", get_PC(z80_p));
#ifdef DEBUG
    if(debug_prompt(gb_emu_p) == ERR_READ_LINE) {
      exit(0);
	}
#endif

    op = dispatch_op(gb_emu_p, &op_store);
    if (op == ERR_OP_INVALID_OR_NOT_IMPLEMENTED) {
      printf("Unimplemented op:\n");
      show_inst_at_addr(gb_emu_p, get_PC(z80_p), NULL);
      exit(0);
    }
    if (op == ERR_INVALID_ADDRESS) {
      printf("Invalid address in op (pc: 0x%04x)\n", get_PC(z80_p));
      printf("%s\n",op_store.mnemonic);
      print_regs(z80_p, 0);
      exit(0);
    }
    if (step_gpu(gb_emu_p) == 1) {
      render(gb_emu_p, window);
      glfwPollEvents();
  }
    handle_interrupts(gb_emu_p);

  }
}
