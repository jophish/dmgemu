#include "opcodes.h"
#include <stdlib.h>
#include <string.h>

int dd_to_reg_pair(cpu *z80_p, int dd_code, reg_pair *reg_pair_p){
  switch (dd_code) {
  case (DD_PAIR_CODE_BC):
    ;
    *reg_pair_p = (reg_pair) {&(z80_p->regs.b), &(z80_p->regs.c)};
    break;
  case (DD_PAIR_CODE_DE):
    ;
    *reg_pair_p = (reg_pair) {&(z80_p->regs.d), &(z80_p->regs.e)};
    break;
  case (DD_PAIR_CODE_HL):
    ;
    *reg_pair_p = (reg_pair) {&(z80_p->regs.h), &(z80_p->regs.l)};
    break;
  case (DD_PAIR_CODE_SP):
    ;
    // this is... a hack
    *reg_pair_p = (reg_pair) {(uint8_t *)&(z80_p->regs.sp), (uint8_t *)((void *)&(z80_p->regs.sp)+1)};
    break;
  default:
    return ERR_INVALID_DD_PAIR;
  }
  return 0;
}

int op_ld_dd_16im(emu *gb_emu_p, int dd_code, uint16_t val) {
  reg_pair regs;
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = dd_to_reg_pair(z80_p, dd_code, &regs)) == ERR_INVALID_DD_PAIR)
    return err;
  *(regs.r1) = (uint8_t)(val >> 8);
  *(regs.r2) = (uint8_t)(val & LSB_MASK);
  return 0;
}

int dispatch_op(emu *gb_emu_p, opcode *op_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t pc = get_PC(z80_p);
  uint16_t op = read_8(gb_emu_p, pc);
  uint8_t cycles;
  uint16_t new_pc, b16_im;
  opcode op_struct;
  int err;
  printf("%d\n", 0xc3);
  if ((CB_MASK & op) == CB_MASK) {
    op = read_8(gb_emu_p, pc+1);
    op_struct = cb_op_array[op];
  } else {
    op_struct = op_array[op];
  }

  *op_p = op_struct;

  // Certain ops conditionally take a variable number of cycles
  // or change the PC in ways not related to opcode length. We set default
  // values to update the CPU fields with here, and change them within the
  // switch case if necessary.
  cycles = op_struct.cyc1;
  new_pc = pc + op_struct.len;

  switch (op_struct.op_type) {
  case (OP_NOP):
    break;
  case (OP_LD_DD_16IM):
    b16_im = read_16(gb_emu_p, pc+1);
    if ((err = op_ld_dd_16im(gb_emu_p, get_dd_code(op), b16_im)) < 0)
      return err;
    break;
  default:

    return ERR_OP_INVALID_OR_NOT_IMPLEMENTED;
  }

  set_PC(z80_p, new_pc);
  z80_p->clk.prev_cpu_cycles = cycles;
  z80_p->clk.prev_m_cycles = cycles/4;
  z80_p->clk.cpu_cycles += cycles;
  z80_p->clk.m_cycles += cycles/4;
  return 0;

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
    case (OP_OR_A) :
      err = sprintf(buf, "or a");
      break;
    case (OP_OR_B) :
      err = sprintf(buf, "or b");
      break;
    case (OP_OR_C) :
      err = sprintf(buf, "or c");
      break;
    case (OP_OR_D) :
      err = sprintf(buf, "or d");
      break;
    case (OP_OR_E) :
      err = sprintf(buf, "or e");
      break;
    case (OP_OR_H) :
      err = sprintf(buf, "or h");
      break;
    case (OP_OR_L) :
      err = sprintf(buf, "or l");
      break;
    case (OP_B16_LD_IV_HL) :
      err = sprintf(buf, "ld hl, 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_B16_LD_IV_SP) :
      err = sprintf(buf, "ld sp, 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_B16_LD_IV_BC) :
      err = sprintf(buf, "ld bc, 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_B16_LD_IV_NN_A) :
      err = sprintf(buf, "ld (0x%04x), a", read_16(gb_emu_p, addr+1));
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
    case (OP_B8_LD_IV_IND_HL) :
      err = sprintf(buf, "ld (hl), 0x%02x", read_8(gb_emu_p, addr+1));
      break;
    case (OP_LDD_HL_A) :
      err = sprintf(buf, "ldd (hl), a");
      break;
    case (OP_LDI_A_HL) :
      err = sprintf(buf, "ldi a, (hl)");
      break;
    case (OP_LD_A_A) :
      err = sprintf(buf, "ld a, a");
      break;
    case (OP_LD_A_B) :
      err = sprintf(buf, "ld a, b");
      break;
    case (OP_LD_A_C) :
      err = sprintf(buf, "ld a, c");
      break;
    case (OP_LD_A_D) :
      err = sprintf(buf, "ld a, d");
      break;
    case (OP_LD_A_E) :
      err = sprintf(buf, "ld a, e");
      break;
    case (OP_LD_A_H) :
      err = sprintf(buf, "ld a, h");
      break;
    case (OP_LD_A_L) :
      err = sprintf(buf, "ld a, l");
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
    case (OP_DEC_BC) :
      err = sprintf(buf, "dec bc");
      break;
    case (OP_DEC_DE) :
      err = sprintf(buf, "dec de");
      break;
    case (OP_DEC_HL) :
      err = sprintf(buf, "dec hl");
      break;
    case (OP_DEC_SP) :
      err = sprintf(buf, "dec sp");
      break;
    case (OP_INC_A) :
      err = sprintf(buf, "inc a");
      break;
    case (OP_INC_B) :
      err = sprintf(buf, "inc b");
      break;
    case (OP_INC_C) :
      err = sprintf(buf, "inc c");
      break;
    case (OP_INC_D) :
      err = sprintf(buf, "inc d");
      break;
    case (OP_INC_E) :
      err = sprintf(buf, "inc e");
      break;
    case (OP_INC_H) :
      err = sprintf(buf, "inc h");
      break;
    case (OP_INC_L) :
      err = sprintf(buf, "inc l");
      break;
    case (OP_INC_IND_HL) :
      err = sprintf(buf, "inc (hl)");
      break;
    case (OP_B8_JR_NZ) :
      ;
      int rel_offset = byte_to_2c(read_8(gb_emu_p, addr+1)) + 2;
      err = sprintf(buf, "jr nz, 0x%04x", addr + rel_offset);
      break;
    case (OP_DI) :
      err = sprintf(buf, "di");
      break;
    case (OP_RET) :
      err = sprintf(buf, "ret");
      break;
    case (OP_B16_CALL_IV) :
      err = sprintf(buf, "call 0x%04x", read_16(gb_emu_p, addr+1));
      break;
    case (OP_LDH_N_A) :
      err = sprintf(buf, "ldh (0x%04x), a", HW_IO_REGS_START + read_8(gb_emu_p, addr+1));
      break;
    case (OP_LDH_A_N) :
      err = sprintf(buf, "ldh a, (0x%04x)", HW_IO_REGS_START + read_8(gb_emu_p, addr+1));
      break;
    case (OP_LDH_C_A) :
      err = sprintf(buf, "ldh (c), a");
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
    case (OP_B8_LD_IV_IND_HL) :
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
    case (OP_B16_LD_IV_NN_A) :
    case (OP_B16_JP_IV) :
    case (OP_B16_CALL_IV) :
      return 3;
    default :
      // catches all 0x4x - 0xBx
      return 1;
  }
}

bool check_hc_add(uint8_t a, uint8_t b) {
  return ((((a & 0xf) + (b + 0xf)) & 0x10) == 0x10);
}
