#include "opcodes.h"
#include <stdlib.h>

uint16_t dispatch_op(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t pc = get_PC(z80_p);
  uint8_t opcode = read_8(gb_emu_p, pc);
  uint8_t val_8, result_8;
  int rel_offset;
  uint16_t val_16;
  switch(opcode) {

    // 8-Bit Immediate Loads
    case (B8_LD_IV_A) :
      // Load Immediate 8-bit value into A
      z80_p->clk.cpu_cycles += 8;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.a = val_8;
      set_PC(z80_p, pc+2);
      break;
    case (B8_LD_IV_B) :
      z80_p->clk.cpu_cycles += 8;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.b = val_8;
      set_PC(z80_p, pc+2);
      break;
    case (B8_LD_IV_C) :
      z80_p->clk.cpu_cycles += 8;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.c = val_8;
      set_PC(z80_p, pc+2);
      break;

    // 16-Bit Immediate Loads
    case (B16_LD_IV_HL) :
      z80_p->clk.cpu_cycles += 12;
      val_16 = read_16(gb_emu_p, pc+1);
      set_HL(z80_p, val_16);
      set_PC(z80_p, pc+3);
      break;

    // NOP
    case (NOP) :
      z80_p->clk.cpu_cycles += 4;
      inc_byte_PC(z80_p);
      break;

    // Jumps
    case (B16_JP_IV) :
      z80_p->clk.cpu_cycles += 12;
      uint16_t new_pc = read_16(gb_emu_p, pc+1);
      set_PC(z80_p, new_pc);
      break;

    case (B8_JR_NZ) :
      // Relative jump if Z == 0
      val_8 = read_8(gb_emu_p, pc+1);
      rel_offset = byte_to_2c(val_8) + 2;
      if (get_flag_Z(z80_p) == 0) {
	set_PC(z80_p, pc + rel_offset);
	z80_p->clk.cpu_cycles += 12;
      } else {
	set_PC(z80_p, pc + 2);
	z80_p->clk.cpu_cycles += 8;
      }
      break;

    // XOR against register A, stores result in A
    case (XOR_A) :
      // Since we're XORing A with A, this is a degenerate case which just sets
      // register A to 0
      z80_p->clk.cpu_cycles += 4;
      z80_p->regs.a = 0;
      set_flag_Z(z80_p);
      reset_flag_N(z80_p);
      reset_flag_H(z80_p);
      reset_flag_C(z80_p);
      set_PC(z80_p, pc+1);
      break;

    case (XOR_B) :
      z80_p->clk.cpu_cycles += 4;
      result_8 = z80_p->regs.b ^ z80_p->regs.a;
      z80_p->regs.a = result_8;
      result_8 == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
      reset_flag_N(z80_p);
      reset_flag_H(z80_p);
      reset_flag_C(z80_p);
      set_PC(z80_p, pc+1);
      break;

    // LDD
    case (LDD_HL_A) :
      // Load A into memory address HL, then decrement HL
      z80_p->clk.cpu_cycles += 8;
      uint16_t address = get_HL(z80_p);
      write_8(gb_emu_p, address, z80_p->regs.a);
      set_HL(z80_p, address-1);
      set_PC(z80_p, pc+1);
      break;

    // DEC
    case (DEC_B) :
      z80_p->clk.cpu_cycles += 4;
      val_8 = z80_p->regs.b;
      if (val_8 == 0) {
	result_8 = 0xff;
      } else {
	result_8 = val_8 - 1;
      }
      z80_p->regs.b = result_8;
      result_8 == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
      set_flag_N(z80_p);
      // If the lower nibble of the original value is less than the lower nibble of what we're subtracting, we need a half-carry
      // https://gist.github.com/Palmr/4526839

      // We can cheat here, since we're always just decrementing.
      (val_8 >> 4) == 0 ? set_flag_H(z80_p) : reset_flag_H(z80_p);
      set_PC(z80_p, pc+1);
      break;

    case (DEC_C) :
      z80_p->clk.cpu_cycles += 4;
      val_8 = z80_p->regs.c;
      if (val_8 == 0) {
	result_8 = 0xff;
      } else {
	result_8 = val_8 - 1;
      }
      z80_p->regs.c = result_8;
      result_8 == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
      set_flag_N(z80_p);
      // If the lower nibble of the original value is less than the lower nibble of what we're subtracting, we need a half-carry
      // https://gist.github.com/Palmr/4526839

      // We can cheat here, since we're always just decrementing.
      (val_8 >> 4) == 0 ? set_flag_H(z80_p) : reset_flag_H(z80_p);
      set_PC(z80_p, pc+1);
      break;

   // Control opcodes
    case (DI) :
      z80_p->clk.cpu_cycles += 4;
      reset_flag_IE(z80_p);
      set_PC(z80_p, pc+1);
      break;
   default :
      printf("Current opcode %02x at ROM address 0x%04x not implemented.\n", opcode, pc);
      exit(0);

  }
  return opcode;
}
