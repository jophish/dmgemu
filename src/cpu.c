#include "cpu.h"
#include <assert.h>
#include <stdio.h>

void init_cpu() {
  set_AF(0x01B0);
  set_BC(0x0013);
  set_DE(0x00D8);
  set_HL(0x014D);
  set_SP(0xFFFE);
  set_PC(0x0100);
}

void print_regs(bool width) {
  if (width) {
    printf("AF: 0x%04x\n", get_AF());
    printf("BC: 0x%04x\n", get_BC());
    printf("DE: 0x%04x\n", get_DE());
    printf("HL: 0x%04x\n", get_HL());
  } else {
    printf("A: 0x%02x, F: 0x%02x\n", z80.regs.a, z80.regs.f);
    printf("B: 0x%02x, C: 0x%02x\n", z80.regs.b, z80.regs.c);
    printf("D: 0x%02x, E: 0x%02x\n", z80.regs.d, z80.regs.e);
    printf("H: 0x%02x, L: 0x%02x\n", z80.regs.h, z80.regs.l);
  }
  printf("SP: 0x%04x\n", get_SP());
  printf("PC: 0x%04x\n", get_PC());
}

void print_flags() {
  printf("Z: %d, N: %d, H: %d, C: %d\n", get_flag_Z(), get_flag_N(), get_flag_H(), get_flag_C());
}

