#include "opcodes.h"
#include <stdlib.h>
#include <string.h>

int dispatch_op(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t pc = get_PC(z80_p);
  uint8_t opcode = read_8(gb_emu_p, pc);
  uint8_t val_8, result_8;
  int rel_offset;
  uint16_t val_16;
  uint16_t new_pc_nj = pc + op_length(opcode); // New PC, given that we don't have a jump
  switch(opcode) {

    /*************************/
    /* 8-Bit Immediate Loads */
    /*************************/
    case (OP_B8_LD_IV_A) :
      // Load Immediate 8-bit value into A
      z80_p->clk.prev_cpu_cycles = 8;
      z80_p->clk.prev_m_cycles = 2;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.a = val_8;
      set_PC(z80_p, new_pc_nj);
      break;
    case (OP_B8_LD_IV_B) :
      z80_p->clk.prev_cpu_cycles = 8;
      z80_p->clk.prev_m_cycles = 2;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.b = val_8;
      set_PC(z80_p, new_pc_nj);
      break;
    case (OP_B8_LD_IV_C) :
      z80_p->clk.prev_cpu_cycles = 8;
      z80_p->clk.prev_m_cycles = 2;
      val_8 = read_8(gb_emu_p, pc+1);
      z80_p->regs.c = val_8;
      set_PC(z80_p, new_pc_nj);
      break;

    /**************************/
    /* 16-Bit Immediate Loads */
    /**************************/
    case (OP_B16_LD_IV_HL) :
      z80_p->clk.prev_cpu_cycles = 12;
      z80_p->clk.prev_m_cycles = 3;
      val_16 = read_16(gb_emu_p, pc+1);
      set_HL(z80_p, val_16);
      set_PC(z80_p, new_pc_nj);
      break;

    /*******/
    /* LDH */
    /*******/
    case (OP_LDH_N_A) :
      z80_p->clk.prev_cpu_cycles = 12;
      z80_p->clk.prev_m_cycles = 3;
      val_8 = read_8(gb_emu_p, pc+1);
      val_16 = HW_IO_REGS_START + val_8;
      write_8(gb_emu_p, val_16, z80_p->regs.a);
      set_PC(z80_p, new_pc_nj);
      break;

    case (OP_LDH_A_N) :
      z80_p->clk.prev_cpu_cycles = 12;
      z80_p->clk.prev_m_cycles = 3;
      val_8 = read_8(gb_emu_p, pc+1);
      result_8 = read_8(gb_emu_p, HW_IO_REGS_START + val_8);
      z80_p->regs.a = result_8;
      set_PC(z80_p, new_pc_nj);
      break;

    /******/
    /* CP */
    /******/
    case (OP_B8_CP_IV_A) :
      z80_p->clk.prev_cpu_cycles = 8;
      z80_p->clk.prev_m_cycles = 2;
      val_8 = read_8(gb_emu_p, pc+1);

      if (val_8 == z80_p->regs.a) {
	set_flag_Z(z80_p);
      } else {
	reset_flag_Z(z80_p);
      }

      if (z80_p->regs.a < val_8) {
	set_flag_C(z80_p);
      } else {
	reset_flag_C(z80_p);
      }

      if ((z80_p->regs.a & NIBBLE_MASK) < (val_8 & NIBBLE_MASK)) {
	set_flag_H(z80_p);
      } else {
	reset_flag_H(z80_p);
      }

      set_flag_N(z80_p);
      set_PC(z80_p, new_pc_nj);
    /*******/
    /* NOP */
    /*******/
    case (OP_NOP) :
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
      set_PC(z80_p, new_pc_nj);
      break;

    /*********/
    /* Jumps */
    /*********/
    case (OP_B16_JP_IV) :
      z80_p->clk.prev_cpu_cycles = 12;
      z80_p->clk.prev_m_cycles = 3;
      uint16_t new_pc = read_16(gb_emu_p, pc+1);
      set_PC(z80_p, new_pc);
      break;

    case (OP_B8_JR_NZ) :
      // Relative jump if Z == 0
      val_8 = read_8(gb_emu_p, pc+1);
      rel_offset = byte_to_2c(val_8) + 2;
      if (get_flag_Z(z80_p) == 0) {
	set_PC(z80_p, pc + rel_offset);
	z80_p->clk.prev_cpu_cycles = 12;
	z80_p->clk.prev_m_cycles = 3;
      } else {
	set_PC(z80_p, new_pc_nj);
	z80_p->clk.prev_cpu_cycles = 8;
	z80_p->clk.prev_m_cycles = 2;
      }
      break;

    /*******/
    /* XOR */
    /*******/
    // XOR against register A, stores result in A
    case (OP_XOR_A) :
      // Since we're XORing A with A, this is a degenerate case which just sets
      // register A to 0
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
      z80_p->regs.a = 0;
      set_flag_Z(z80_p);
      reset_flag_N(z80_p);
      reset_flag_H(z80_p);
      reset_flag_C(z80_p);
      set_PC(z80_p, new_pc_nj);
      break;

    case (OP_XOR_B) :
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
      result_8 = z80_p->regs.b ^ z80_p->regs.a;
      z80_p->regs.a = result_8;
      result_8 == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
      reset_flag_N(z80_p);
      reset_flag_H(z80_p);
      reset_flag_C(z80_p);
      set_PC(z80_p, new_pc_nj);
      break;

    /*******/
    /* LDD */
    /*******/
    case (OP_LDD_HL_A) :
      // Load A into memory address HL, then decrement HL
      z80_p->clk.prev_cpu_cycles = 8;
      z80_p->clk.prev_m_cycles = 2;
      uint16_t address = get_HL(z80_p);
      write_8(gb_emu_p, address, z80_p->regs.a);
      set_HL(z80_p, address-1);
      set_PC(z80_p, new_pc_nj);
      break;

    /*******/
    /* DEC */
    /*******/
    case (OP_DEC_B) :
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
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
      set_PC(z80_p, new_pc_nj);
      break;

    case (OP_DEC_C) :
      z80_p->clk.prev_m_cycles = 1;
      z80_p->clk.prev_cpu_cycles = 4;
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
      set_PC(z80_p, new_pc_nj);
      break;

    /***********/
    /* Control */
    /***********/
    case (OP_DI) :
      z80_p->clk.prev_cpu_cycles = 4;
      z80_p->clk.prev_m_cycles = 1;
      reset_flag_IME(z80_p);
      set_PC(z80_p, new_pc_nj);
      break;
   default :
      printf("Current opcode %02x at ROM address 0x%04x not implemented.\n", opcode, pc);
      return ERR_OP_INVALID_OR_NOT_IMPLEMENTED;

  }
  // Update the global clock based on how many cycles we just used
  z80_p->clk.cpu_cycles += z80_p->clk.prev_cpu_cycles;
  z80_p->clk.m_cycles += z80_p->clk.prev_m_cycles;
  return opcode;
}

int addr_to_op_str(emu *gb_emu_p, uint16_t addr, char *buf, int buf_len) {
  uint8_t opcode = read_8(gb_emu_p, addr);
  int err = 0;
  switch (opcode) {
    case (OP_NOP) :
      err = sprintf(buf, "nop");
      break;
    case (OP_B16_JP_IV) :
      err = sprintf(buf, "jp 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_XOR_A) :
      err = sprintf(buf, "xor a");
      break;
    case (OP_B16_LD_IV_HL) :
      err = sprintf(buf, "ld hl, 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_A) :
      err = sprintf(buf, "ld a, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_B) :
      err = sprintf(buf, "ld b, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_C) :
      err = sprintf(buf, "ld c, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_D) :
      err = sprintf(buf, "ld d, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_E) :
      err = sprintf(buf, "ld e, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_H) :
      err = sprintf(buf, "ld h, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_LD_IV_L) :
      err = sprintf(buf, "ld l, 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_LDD_HL_A) :
      err = sprintf(buf, "ldd (hl), a");
      break;
    case (OP_DEC_A) :
      err = sprintf(buf, "dec a");
      break;
    case (OP_DEC_B) :
      err = sprintf(buf, "dec b");
      break;
    case (OP_DEC_C) :
      err = sprintf(buf, "dec c");
      break;
    case (OP_DEC_D) :
      err = sprintf(buf, "dec d");
      break;
    case (OP_DEC_E) :
      err = sprintf(buf, "dec e");
      break;
    case (OP_DEC_H) :
      err = sprintf(buf, "dec h");
      break;
    case (OP_DEC_L) :
      err = sprintf(buf, "dec l");
      break;
    case (OP_DEC_IND_HL) :
      err = sprintf(buf, "dec (hl)");
      break;
    case (OP_B8_JR_NZ) :
      ;
      int rel_offset = byte_to_2c(read_8(gb_emu_p, addr+1)) + 2;
      err = sprintf(buf, "jr nz, 0x%04x", addr + rel_offset);
      break;
    case (OP_DI) :
      err = sprintf(buf, "di");
      break;
    case (OP_LDH_N_A) :
      err = sprintf(buf, "ldh (0x%04x), a", HW_IO_REGS_START + read_8(gb_emu_p, addr+1));
      break;
    case (OP_LDH_A_N) :
      err = sprintf(buf, "ldh a, (0x%04x)", HW_IO_REGS_START + read_8(gb_emu_p, addr+1));
      break;
    case (OP_B8_CP_IV_A) :
      err = sprintf(buf, "cp (0x%02x)", read_8(gb_emu_p, addr+1));
      break;
    default :
      return ERR_OP_INVALID_OR_NOT_IMPLEMENTED;
  }
  if (err < 0) {
    return ERR_BUF_LEN;
  }
  return 0;
}

int op_length(uint16_t op) {
  if ((op & BYTE_MASK) == PREFIX_CB) {
    return 2;
  }

  switch (op) {
    // 2 bytes
    case (OP_B8_LD_IV_A) :
    case (OP_B8_LD_IV_B) :
    case (OP_B8_LD_IV_C) :
    case (OP_B8_LD_IV_D) :
    case (OP_B8_LD_IV_E) :
    case (OP_B8_LD_IV_H) :
    case (OP_B8_LD_IV_L) :
    case (OP_LDH_N_A) :
    case (OP_LDH_A_N) :
    case (OP_B8_JR_NZ) :
    case (OP_B8_CP_IV_A) :
      return 2;
    // 3 bytes
    case (OP_B16_LD_IV_BC) :
    case (OP_B16_LD_IV_DE) :
    case (OP_B16_LD_IV_HL) :
    case (OP_B16_LD_IV_SP) :
    case (OP_B16_JP_IV) :
      return 3;
    default :
      // catches all 0x4x - 0xBx
      return 1;
  }
}
