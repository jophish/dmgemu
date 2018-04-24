#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "cpu.h"
#include "rom.h"
#include "timer.h"
#include "opcodes.h"
#include "emu.h"
#include "debug.h"
#include "error.h"
#include "gpu.h"
#include "window.h"
#include "int.h"

#include <GLFW/glfw3.h>

struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

int main(int argc, char **argv) {
  if (argc == 1) {
    exit(-1);
  }

  opcode op_store;
  int op;
  emu gb_emu;

  emu *gb_emu_p = &gb_emu;
  cpu *z80_p = &(gb_emu.z80);
  rom *gb_rom_p = &(gb_emu.gb_rom);

  // Initializing emu stuff
  init_emu(gb_emu_p);
  load_rom(gb_rom_p, argv[1]);
  set_PC(z80_p, 0x100);

  // Setting up windows and timers
  GLFWwindow *window = init_window(gb_emu_p);
  double ns_per_cycle = 238.46998;
  uint8_t cycles_taken;
  struct timespec timer, timer2;
  timer2 = timer_start();

  while (true) {

#ifdef DEBUG
    if(debug_prompt(gb_emu_p) == ERR_READ_LINE) {
      exit(0);
    }
#endif

    timer = timer_start();
    if (z80_p->halt == false) {
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
    } else {
      // if we've halted, just tick up the clock until something breaks us out of halt
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
      z80_p->clk.cpu_cycles += 4;
      z80_p->clk.m_cycles += 1;
    }

    if (step_gpu(gb_emu_p) == 1) {
      render(gb_emu_p, window);
      glfwPollEvents();
      long int time_taken = timer_end(timer2);
    }

    update_timer(gb_emu_p);
    handle_interrupts(gb_emu_p);

    // For now, we just spin in order to get a decently correct framerate
    cycles_taken = z80_p->clk.prev_cpu_cycles;
    long int cyc_ns_time = (long int)(cycles_taken*ns_per_cycle);
    long int end;
    while((end = timer_end(timer)) < cyc_ns_time*.7) {}
  }
}
