#include "cpu.h"
#include <assert.h>
#include <stdio.h>

void init_cpu(cpu *z80) {
  set_AF(z80, 0x01B0);
  set_BC(z80, 0x0013);
  set_DE(z80, 0x00D8);
  set_HL(z80, 0x014D);
  set_SP(z80, 0xFFFE);
  set_PC(z80, 0x0100);
  set_flag_IME(z80);
  z80->halt = false;
  z80->regs.ei_enable = false;
}

void print_regs(cpu *z80, bool width) {
  if (width) {
    printf("AF: 0x%04x\n", get_AF(z80));
    printf("BC: 0x%04x\n", get_BC(z80));
    printf("DE: 0x%04x\n", get_DE(z80));
    printf("HL: 0x%04x\n", get_HL(z80));
  } else {
    printf("A: 0x%02x, F: 0x%02x\n", z80->regs.a, z80->regs.f);
    printf("B: 0x%02x, C: 0x%02x\n", z80->regs.b, z80->regs.c);
    printf("D: 0x%02x, E: 0x%02x\n", z80->regs.d, z80->regs.e);
    printf("H: 0x%02x, L: 0x%02x\n", z80->regs.h, z80->regs.l);
  }
  printf("SP: 0x%04x\n", get_SP(z80));
  printf("PC: 0x%04x\n", get_PC(z80));
  printf("IME: %s\n", z80->regs.ime ? "true" : "false");
}

void print_flags(cpu *z80) {
  printf("Z: %d, N: %d, H: %d, C: %d\n", get_flag_Z(z80), get_flag_N(z80), get_flag_H(z80), get_flag_C(z80));
}

