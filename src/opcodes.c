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
    *reg_pair_p = (reg_pair) {&(z80_p->regs.sp_hi), &(z80_p->regs.sp_lo)};
    break;
  default:
    return ERR_INVALID_DD_PAIR;
  }
  return 0;
}

int reg_code_to_pointer(cpu *z80_p, int reg_code, uint8_t **reg_p) {
  switch (reg_code) {
  case (REG_CODE_A):
    *reg_p = &(z80_p->regs.a);
    break;
  case (REG_CODE_B):
    *reg_p = &(z80_p->regs.b);
    break;
  case (REG_CODE_C):
    *reg_p = &(z80_p->regs.c);
    break;
  case (REG_CODE_D):
    *reg_p = &(z80_p->regs.d);
    break;
  case (REG_CODE_E):
    *reg_p = &(z80_p->regs.e);
    break;
  case (REG_CODE_H):
    *reg_p = &(z80_p->regs.h);
    break;
  case (REG_CODE_L):
    *reg_p = &(z80_p->regs.l);
    break;
  default:
    return ERR_INVALID_REG_CODE;
  };
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

int op_xor_a_r(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  z80_p->regs.a = z80_p->regs.a ^ *reg_p;
  reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
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

  addr_to_op(gb_emu_p, pc, &op_struct);

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
  case (OP_JP_16IM):
    new_pc = read_16(gb_emu_p, pc+1);
    break;
  case (OP_XOR_REG):
    if ((err = op_xor_a_r(gb_emu_p, get_reg_code(op))) < 0)
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

bool special_arg(uint8_t arg) {
  switch (arg){
  case (ARG_D16):
  case (ARG_D8):
  case (ARG_IND_A16):
  case (ARG_IND_A8):
  case (ARG_R8):
  case (ARG_A8):
  case (ARG_A16):
  case (ARG_SPIR8):
    return true;
  default:
    return false;
  }
}

int addr_to_op(emu *gb_emu_p, uint16_t addr, opcode *op_struct_p) {
  uint8_t op = read_8(gb_emu_p, addr);
  if ((CB_MASK & op) == CB_MASK) {
    op = read_8(gb_emu_p, addr+1);
    *op_struct_p = cb_op_array[op];
  } else {
    *op_struct_p = op_array[op];
  }
  return 0;
}

int addr_to_op_str(emu *gb_emu_p, uint16_t addr, char *buf, int buf_len, opcode *op_struct_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t pc = get_PC(z80_p);
  int switch_arg = -1;
  opcode op_struct;
  int err;

  addr_to_op(gb_emu_p, addr, &op_struct);
  *op_struct_p =  op_struct;

  if (special_arg(op_struct.arg1)) {
      switch_arg = op_struct.arg1;
    } else if (special_arg(op_struct.arg2)) {
      switch_arg = op_struct.arg2;
    }

  if (switch_arg > 0) {
    uint8_t im8 = read_8(gb_emu_p, addr+1);
    uint16_t im16 = read_16(gb_emu_p, addr+1);
    uint16_t format;

    switch (switch_arg) {
    case (ARG_D16):
    case (ARG_A16):
    case (ARG_IND_A16):
      format = im16;
      break;
    case (ARG_D8):
    case (ARG_IND_A8):
      format = (uint16_t)im8;
      break;
    case (ARG_R8):
      format = pc + byte_to_2c(im8);
      break;
    case (ARG_A8):
      format = HW_IO_REGS_START + im8;
      break;
    case (ARG_SPIR8):
      format = get_SP(z80_p) + byte_to_2c(im8);
      break;
    default:
	printf("Error while formatting debug string.\n");
	exit(0);
    }

    if ((err = snprintf(buf, buf_len, op_struct.mnemonic, format)) < 0)
      return ERR_BUF_LEN;

    return 0;
  }

  if ((err = snprintf(buf, buf_len, op_struct.mnemonic)) < 0) {
    return ERR_BUF_LEN;
  }
  return 0;
}

bool check_hc_add(uint8_t a, uint8_t b) {
  return ((((a & 0xf) + (b + 0xf)) & 0x10) == 0x10);
}
