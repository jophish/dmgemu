#include "opcodes.h"
#include <stdlib.h>
#include <string.h>

int t_to_addr(int t_code) {
  uint8_t addr;
  switch (t_code) {
  case (T_CODE_00):
    addr = 0;
    break;
  case (T_CODE_08):
    addr = 0x08;
    break;
  case (T_CODE_10):
    addr = 0x10;
    break;
  case (T_CODE_18):
    addr = 0x18;
    break;
  case (T_CODE_20):
    addr = 0x20;
    break;
  case (T_CODE_28):
    addr = 0x28;
    break;
  case (T_CODE_30):
    addr = 0x30;
    break;
  case (T_CODE_38):
    addr = 0x38;
    break;
  default:
    return ERR_INVALID_T_CODE;
  }
  return addr;
}

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

int ss_to_reg_pair(cpu *z80_p, int ss_code, uint16_t **reg_pair_pp) {
  switch (ss_code) {
  case (SS_PAIR_CODE_BC):
    ;
    *reg_pair_pp = &(z80_p->regs.bc);
    break;
  case (SS_PAIR_CODE_DE):
    ;
    *reg_pair_pp = &(z80_p->regs.de);
    break;
  case (SS_PAIR_CODE_HL):
    ;
    *reg_pair_pp = &(z80_p->regs.hl);
    break;
  case (SS_PAIR_CODE_SP):
    ;
    *reg_pair_pp = &(z80_p->regs.sp);
    break;
  default:
    return ERR_INVALID_SS_PAIR;
  }
  return 0;
}

int qq_to_reg_pair(cpu *z80_p, int qq_code, uint16_t **reg_pair_pp) {
  switch (qq_code) {
  case (QQ_PAIR_CODE_BC):
    ;
    *reg_pair_pp = &(z80_p->regs.bc);
    break;
  case (QQ_PAIR_CODE_DE):
    ;
    *reg_pair_pp = &(z80_p->regs.de);
    break;
  case (QQ_PAIR_CODE_HL):
    ;
    *reg_pair_pp = &(z80_p->regs.hl);
    break;
  case (QQ_PAIR_CODE_AF):
    ;
    *reg_pair_pp = &(z80_p->regs.af);
    break;
  default:
    return ERR_INVALID_QQ_PAIR;
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

int op_cp_a_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  uint8_t *reg_p;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_cp_a_8im(gb_emu_p, *reg_p);
}

int op_cp_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_cp_a_8im(gb_emu_p, val);
}

int op_cp_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t result = z80_p->regs.a - val;

  (((z80_p->regs.a & 0xF) - (val & 0xF)) < 0) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  result == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  val > z80_p->regs.a ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  set_flag_N(z80_p);

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

int op_xor_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  z80_p->regs.a = z80_p->regs.a ^ val;
  reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_ld_r_8im(emu *gb_emu_p, int reg_code, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  *reg_p = val;
  return 0;
}

int op_ld_ind_hld_a(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = get_HL(z80_p);
  int err;
  if ((err = write_8(gb_emu_p, addr, z80_p->regs.a)) < 0)
    return err;
  set_HL(z80_p, addr-1);
  return 0;
}

int op_ld_ind_8im_a(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = HW_IO_REGS_START + val;

  write_8(gb_emu_p, addr, z80_p->regs.a);
  return 0;
}

int op_ld_a_ind_8im(emu *gb_emu_p, uint8_t addr) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t full_addr = HW_IO_REGS_START + addr;
  uint8_t val;
  if ((val = read_8(gb_emu_p, full_addr)) < 0)
    return val;
  z80_p->regs.a = val;
  return 0;
}

int op_dec_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;

  set_flag_N(z80_p);
  ((*reg_p  - 1) == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  (((*reg_p & 0xF) - (0x1 & 0xF)) < 0) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  *reg_p = *reg_p - 1;
  return 0;
}

int op_ld_ind_hl_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = get_HL(z80_p);
  int err;
  if ((err = write_8(gb_emu_p, addr, val)) < 0)
    return err;
  return 0;
}

int op_ld_ind_16im_a(emu *gb_emu_p, uint16_t addr) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = write_8(gb_emu_p, addr, z80_p->regs.a)) < 0)
    return err;
  return 0;
}

int op_ld_a_ind_hli(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  z80_p->regs.a = val;
  set_HL(z80_p, (get_HL(z80_p)) + 1);
  return 0;
}

int op_ld_ind_c_a(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = HW_IO_REGS_START + z80_p->regs.c;
  int err;
  if ((err = write_8(gb_emu_p, addr, z80_p->regs.a)) < 0)
    return err;
  return 0;
}

int op_dec_ss(emu *gb_emu_p, int ss_code) {
  uint16_t *reg_p;
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = ss_to_reg_pair(z80_p, ss_code, &reg_p)) < 0)
    return err;
  *reg_p = *reg_p - 1;
  return 0;
}

int op_inc_ss(emu *gb_emu_p, int ss_code) {
  uint16_t *reg_p;
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = ss_to_reg_pair(z80_p, ss_code, &reg_p)) < 0)
    return err;

  *reg_p = *reg_p + 1;

  return 0;
}

int op_ld_r1_r2(emu *gb_emu_p, int reg_code1, int reg_code2) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *dst_p, *src_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code1, &dst_p)) < 0)
    return err;
  if ((err = reg_code_to_pointer(z80_p, reg_code2, &src_p)) < 0)
    return err;
  *dst_p = *src_p;
  return 0;
}

int op_or_a_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;

  z80_p->regs.a = z80_p->regs.a | *reg_p;

  reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_cpl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  z80_p->regs.a = ~(z80_p->regs.a);
  set_flag_N(z80_p);
  set_flag_H(z80_p);
  return 0;
}

int op_and_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  z80_p->regs.a = (z80_p->regs.a) & val;
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  reset_flag_N(z80_p);
  set_flag_H(z80_p);
  reset_flag_C(z80_p);
  return 0;
}

int op_and_a_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_and_a_8im(gb_emu_p, *reg_p);
}

int op_and_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val = read_8(gb_emu_p, z80_p->regs.hl);
  if (val < 0)
    return val;
  return(op_and_a_8im(gb_emu_p, val));
}

int op_swap_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;

  *reg_p = ((*reg_p) << 4) | ((*reg_p) >> 4);

  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  reset_flag_N(z80_p);
  reset_flag_H(z80_p);
  reset_flag_C(z80_p);
  return 0;
}

int op_swap_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t addr = get_HL(z80_p);
  uint8_t val = read_8(gb_emu_p, addr);
  if (val < 0)
    return val;

  uint8_t result = (val << 4) | (val >> 4);
  write_8(gb_emu_p, addr, result);

  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  reset_flag_N(z80_p);
  reset_flag_H(z80_p);
  reset_flag_C(z80_p);
  return 0;
}

int op_add_a_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_add_a_8im(gb_emu_p, *reg_p);
}

int op_add_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t res = z80_p->regs.a + val;
  reset_flag_H(z80_p);
  check_hc_add(z80_p->regs.a, val) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (res > 0xff) ? set_flag_C(z80_p) : reset_flag_C(z80_p);

  z80_p->regs.a = z80_p->regs.a + val;

  z80_p->regs.a == 0 ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_add_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = z80_p->regs.hl;
  int val = read_8(gb_emu_p, addr);
  if (val < 0)
    return val;
  return op_add_a_8im(gb_emu_p, val);
}

int op_pop_qq(emu *gb_emu_p, int qq_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t *reg_pair_p;
  int err;
  if ((err = qq_to_reg_pair(z80_p, qq_code, &reg_pair_p)) < 0)
    return err;
  uint16_t sp = get_SP(z80_p);
  *reg_pair_p = read_16(gb_emu_p, sp);
  set_SP(z80_p, sp + 2);
  return 0;
}

int op_push_qq(emu *gb_emu_p, int qq_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t *reg_pair_p;
  int err;
  if ((err = qq_to_reg_pair(z80_p, qq_code, &reg_pair_p)) < 0)
    return err;
  uint16_t sp = get_SP(z80_p);
  if ((err = write_8(gb_emu_p, sp-1, (uint8_t)(*reg_pair_p >> 8))) < 0)
    return err;
  if ((err = write_8(gb_emu_p, sp-2, (uint8_t)(*reg_pair_p & 0xFF))) < 0)
    return err;
  set_SP(z80_p, sp - 2);
  return 0;
}

int op_add_hl_ss(emu *gb_emu_p, int ss_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t hl_val = get_HL(z80_p);
  uint16_t *reg_pair_p;
  int err;
  if ((err = ss_to_reg_pair(z80_p, ss_code, &reg_pair_p)) < 0)
    return err;
  reset_flag_H(z80_p);
  // set H flag if carry from bit 11
  ((((hl_val & 0xFFF) + (*reg_pair_p & 0xFFF)) & 0x1000) == 0x1000) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  // set C flag if carry from bit 15
  (((hl_val + *reg_pair_p) & 0x10000) == 0x10000) ? set_flag_C(z80_p) : reset_flag_C(z80_p);

  set_HL(z80_p, hl_val + *reg_pair_p);
  return 0;
}

int op_ld_r_ind_hl(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  uint8_t val;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  *reg_p = val;
  return 0;
}

int op_res_bit_r1(emu *gb_emu_p, uint8_t bit, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  *reg_p &= ~(1UL << bit);
  return 0;
}

int op_res_bit_ind_hl(emu *gb_emu_p, uint8_t bit) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  val &= ~(1UL << bit);
  if ((val = write_8(gb_emu_p, get_HL(z80_p), val)) < 0)
    return val;
  return 0;
}

int op_ld_ind_de_a(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = write_8(gb_emu_p, get_DE(z80_p), z80_p->regs.a)) < 0)
    return err;
  return 0;
}

int op_ld_ind_bc_a(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = write_8(gb_emu_p, get_BC(z80_p), z80_p->regs.a)) < 0)
    return err;
  return 0;
}

int op_ld_a_ind_de(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_DE(z80_p))) < 0)
    return val;
  z80_p->regs.a = val;
  return 0;
}

int op_ld_a_ind_bc(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_BC(z80_p))) < 0)
    return val;
  z80_p->regs.a = val;
  return 0;
}

int op_ld_ind_hli_a(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = write_8(gb_emu_p, get_HL(z80_p), z80_p->regs.a)) < 0)
    return err;
  set_HL(z80_p, (get_HL(z80_p)) + 1);
  return 0;
}

int op_ld_a_ind_16im(emu *gb_emu_p, uint16_t addr) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, addr)) < 0)
    return val;
  z80_p->regs.a = val;
  return 0;
}

int op_inc_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  check_hc_add(*reg_p, 0x01) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  reset_flag_H(z80_p);

  *reg_p = *reg_p + 0x01;

  (*reg_p == 0x00) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_sub_a_r1( emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  (z80_p->regs.a & 0x0f) < (*reg_p & 0x0f) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (z80_p->regs.a < *reg_p) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  z80_p->regs.a = z80_p->regs.a - *reg_p;
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_N(z80_p);
  return 0;
}

int op_sub_a_8im( emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  (z80_p->regs.a & 0x0f) < (val & 0x0f) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (z80_p->regs.a < val) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  z80_p->regs.a = z80_p->regs.a - val;
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_N(z80_p);
  return 0;
}

int op_dec_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  set_flag_N(z80_p);
  ((val  - 1) == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  (((val & 0xF) - (0x1 & 0xF)) < 0) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  write_8(gb_emu_p, get_HL(z80_p), val - 1);
  return 0;
}

int op_ld_ind_hl_r(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  if ((err = write_8(gb_emu_p, get_HL(z80_p), *reg_p)) < 0)
    return err;
  return 0;
}

int op_inc_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  set_flag_N(z80_p);
  ((val  + 1) == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  (((val & 0xF) + (0x1 & 0xF)) > 0xF) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  write_8(gb_emu_p, get_HL(z80_p), val + 1);
  return 0;
}

int op_srl_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  ((*reg_p & 0x1) == 0x1) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);

  *reg_p = *reg_p >> 1;
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_srl_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  uint8_t result = val >> 1;

  ((val & 0x1) == 0x1) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);

  write_8(gb_emu_p, get_HL(z80_p), result);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_bit_num_r1(emu *gb_emu_p, uint8_t bit, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  uint8_t b = (*reg_p >> bit) & 0x01;
  (b == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_H(z80_p);
  reset_flag_N(z80_p);
  return 0;
}

int dispatch_op(emu *gb_emu_p, opcode *op_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t pc = get_PC(z80_p);
  uint16_t op = read_8(gb_emu_p, pc);
  uint8_t cycles, b8_im;
  uint16_t new_pc, b16_im, sp;
  opcode op_struct;
  int err;
  //printf("PC: 0x%04x\n", pc);
  // If we have a CB op, use the next address as opcode
  if (addr_to_op(gb_emu_p, pc, &op_struct)) {
    op = read_8(gb_emu_p, pc+1);
  }
  *op_p = op_struct;

  // Certain ops conditionally take a variable number of cycles
  // or change the PC in ways not related to opcode length. We set default
  // values to update the CPU fields with here, and change them within the
  // switch case if necessary.
  cycles = op_struct.cyc1;
  new_pc = pc + op_struct.len;

  //printf("old pc: 0x%04x. new pc: 0x%04x\n", pc, new_pc);
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
    if ((err = op_xor_a_r(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_LD_REG_8IM):
    b8_im = read_8(gb_emu_p, pc+1);
    if ((err = op_ld_r_8im(gb_emu_p, get_reg_code_hi(op), b8_im)) < 0)
      return err;
    break;
  case (OP_LD_IND_HLD_A):
    if ((err = op_ld_ind_hld_a(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_DEC_REG):
    if ((err = op_dec_r1(gb_emu_p, get_reg_code_hi(op))) < 0)
      return err;
    break;
  case (OP_JR_NZ_8IM):
    if (get_flag_Z(z80_p) == 0) {
      new_pc = pc + byte_to_2c(read_8(gb_emu_p, pc+1)) + 2;
      cycles = op_struct.cyc2;
    }
    break;
  case (OP_CP_A_REG):
    if ((err = op_cp_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_CP_A_IND_HL):
    if ((err = op_cp_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_CP_A_8IM):
    if ((err = op_cp_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_DI):
    reset_flag_IME(z80_p);
    break;
  case (OP_LDH_8IM_A):
    b8_im = read_8(gb_emu_p, pc+1);
    if ((err = op_ld_ind_8im_a(gb_emu_p, b8_im)) < 0)
      return err;
    break;
  case (OP_LDH_A_8IM):
    b8_im = read_8(gb_emu_p, pc+1);
    if ((err = op_ld_a_ind_8im(gb_emu_p, b8_im)) < 0)
      return err;
    break;
  case (OP_LD_IND_HL_8IM):
    b8_im = read_8(gb_emu_p, pc+1);
    if ((err = op_ld_ind_hl_8im(gb_emu_p, b8_im)) < 0)
      return err;
    break;
  case (OP_LD_IND_16IM_A):
    b16_im = read_16(gb_emu_p, pc+1);
    if ((err = op_ld_ind_16im_a(gb_emu_p, b16_im)) < 0)
      return err;
    break;
  case (OP_LD_A_IND_HLI):
    if ((err = op_ld_a_ind_hli(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_IND_C_A):
    if ((err = op_ld_ind_c_a(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_CALL_16IM):
    ;
    sp = get_SP(z80_p);
    if ((err = write_8(gb_emu_p, sp-1, new_pc >> 8)) < 0)
      return err;
    if ((err = write_8(gb_emu_p, sp-2, (new_pc & 0x00FF))) < 0)
      return err;
    new_pc = read_16(gb_emu_p, pc+1);
    set_SP(z80_p, sp - 2);
    break;
  case (OP_DEC_SS):
    if ((err = op_dec_ss(gb_emu_p, get_ss_code(op))) < 0)
      return err;
    break;
  case (OP_LD_R1_R2):
    if ((err = op_ld_r1_r2(gb_emu_p, get_reg_code_hi(op), get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_OR_REG):
    if ((err = op_or_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RET):
    ;
    new_pc = read_16(gb_emu_p, get_SP(z80_p));
    set_SP(z80_p, (get_SP(z80_p) + 2));
    break;
  case (OP_EI):
    set_flag_IME(z80_p);
    break;
  case (OP_CPL):
    if ((err = op_cpl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_AND_A_8IM):
    b8_im = read_8(gb_emu_p, pc+1);
    if ((err = op_and_a_8im(gb_emu_p, b8_im)) < 0)
      return err;
    break;
  case (OP_AND_A_REG):
    if ((err = op_and_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_AND_A_IND_HL):
    if ((err = op_and_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_SWAP_REG):
    if ((err = op_swap_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_SWAP_IND_HL):
    if ((err = op_swap_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RST):
    ;
    int jp_addr = t_to_addr(get_t_code(op));
    if (jp_addr < 0)
      return jp_addr;
    uint16_t next_pc = pc + op_struct.len;
    sp = get_SP(z80_p);
    if ((err = write_8(gb_emu_p, sp-1, next_pc >> 8)) < 0)
      return err;
    if ((err = write_8(gb_emu_p, sp-2, (next_pc & 0x00FF))) < 0)
      return err;
    new_pc = (0x00 << 4) | (uint8_t)jp_addr;
    set_SP(z80_p, sp - 2);
    break;
  case (OP_ADD_A_REG):
    if ((err = op_add_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_ADD_A_8IM):
    if ((err = op_add_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_ADD_A_IND_HL):
    if ((err = op_add_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_POP):
    if ((err = op_pop_qq(gb_emu_p, get_qq_code(op))) < 0)
      return err;
    break;
  case (OP_ADD_HL_SS):
    if ((err = op_add_hl_ss(gb_emu_p, get_ss_code(op))) < 0)
      return err;
    break;
  case (OP_LD_REG_IND_HL):
    if ((err = op_ld_r_ind_hl(gb_emu_p, get_reg_code_hi(op))) < 0)
      return err;
    break;
  case (OP_INC_SS):
    if ((err = op_inc_ss(gb_emu_p, get_ss_code(op))) < 0)
      return err;
    break;
  case (OP_PUSH):
    if ((err = op_push_qq(gb_emu_p, get_qq_code(op))) < 0)
      return err;
    break;
  case (OP_JP_IND_HL):
    new_pc = get_HL(z80_p);
    break;
  case (OP_RES_BIT_REG):
    if ((err = op_res_bit_r1(gb_emu_p, get_bit_offset(op), get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RES_BIT_IND_HL):
    if ((err = op_res_bit_ind_hl(gb_emu_p, get_bit_offset(op))) < 0)
      return err;
    break;
  case (OP_LD_IND_DE_A):
    if ((err = op_ld_ind_de_a(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_IND_BC_A):
    if ((err = op_ld_ind_bc_a(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_A_IND_DE):
    if ((err = op_ld_a_ind_de(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_A_IND_BC):
    if ((err = op_ld_a_ind_bc(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_IND_HLI_A):
    if ((err = op_ld_ind_hli_a(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_A_IND_IM16):
    if ((err = op_ld_a_ind_16im(gb_emu_p, read_16(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_JR_Z_8IM):
    if (get_flag_Z(z80_p) == 1) {
      new_pc = pc + byte_to_2c(read_8(gb_emu_p, pc+1)) + 2;
      cycles = op_struct.cyc2;
    }
    break;
  case (OP_INC_REG):
    if ((err = op_inc_r1(gb_emu_p, get_reg_code_hi(op))) < 0)
      return err;
    break;
  case (OP_JP_Z_16IM):
    if (get_flag_Z(z80_p) == 1) {
      new_pc = read_16(gb_emu_p, pc+1);
      cycles = op_struct.cyc2;
    }
    break;
  case (OP_JP_NZ_16IM):
    if (get_flag_Z(z80_p) == 0) {
      new_pc = read_16(gb_emu_p, pc+1);
      cycles = op_struct.cyc2;
    }
    break;
  case (OP_RET_Z):
    if (get_flag_Z(z80_p) == 1) {
      new_pc = read_16(gb_emu_p, get_SP(z80_p));
      set_SP(z80_p, (get_SP(z80_p) + 2));
    }
    break;
  case (OP_JR_8IM):
    new_pc = pc + byte_to_2c(read_8(gb_emu_p, pc+1)) + 2;
    break;
  case (OP_SUB_REG):
    if ((err = op_sub_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_DEC_IND_HL):
    if ((err = op_dec_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RETI):
    new_pc = read_16(gb_emu_p, get_SP(z80_p));
    set_SP(z80_p, (get_SP(z80_p) + 2));
    z80_p->regs.ime = 1;
    break;
  case (OP_LD_IND_HL_REG):
    if ((err = op_ld_ind_hl_r(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RET_NZ):
    if (get_flag_Z(z80_p) == 0) {
      new_pc = read_16(gb_emu_p, get_SP(z80_p));
      set_SP(z80_p, (get_SP(z80_p) + 2));
    }
    break;
  case (OP_INC_IND_HL):
    if ((err = op_inc_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_CALL_NZ_16IM):
    if (get_flag_Z(z80_p) != 0)
      break;
    sp = get_SP(z80_p);
    if ((err = write_8(gb_emu_p, sp-1, new_pc >> 8)) < 0)
      return err;
    if ((err = write_8(gb_emu_p, sp-2, (new_pc & 0x00FF))) < 0)
      return err;
    new_pc = read_16(gb_emu_p, pc+1);
    set_SP(z80_p, sp - 2);
    break;
  case (OP_SUB_8IM):
    if ((err = op_sub_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_XOR_IND_HL):
    if ((err = op_xor_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_SRL_REG):
    if ((err = op_srl_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_SRL_IND_HL):
    if ((err = op_srl_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_BIT_NUM_REG):
    if ((err = op_bit_num_r1(gb_emu_p, get_bit_offset(op), get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_JR_NC_8IM):
    if (get_flag_C(z80_p) == 0)
      new_pc = pc + byte_to_2c(read_8(gb_emu_p, pc+1)) + 2;
    break;
  case (OP_JR_C_8IM):
    if (get_flag_C(z80_p) != 0)
      new_pc = pc + byte_to_2c(read_8(gb_emu_p, pc+1)) + 2;
    break;
  case (OP_JP_C_16IM):
    if (get_flag_C(z80_p) != 0)
      new_pc = read_16(gb_emu_p, pc+1);
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

bool addr_to_op(emu *gb_emu_p, uint16_t addr, opcode *op_struct_p) {
  uint8_t op = read_8(gb_emu_p, addr);
  if (op == CB_MASK) {
    op = read_8(gb_emu_p, addr+1);
    *op_struct_p = cb_op_array[op];
    return true;
  } else {
    *op_struct_p = op_array[op];
    return false;
  }
}

int addr_to_op_str(emu *gb_emu_p, uint16_t addr, char *buf, int buf_len, opcode *op_struct_p) {
  cpu *z80_p = &(gb_emu_p->z80);
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
      format = (uint16_t)im8;
      break;
    case (ARG_R8):
      format = addr + 2 + byte_to_2c(im8);    // addr is actually pc
      break;
    case (ARG_IND_A8):
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

  strncpy(buf, op_struct.mnemonic, buf_len);
  return 0;
}

bool check_hc_add(uint8_t a, uint8_t b) {
  return ((((a & 0xf) + (b + 0xf)) & 0x10) == 0x10);
}
