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
  return op_xor_a_8im(gb_emu_p, *reg_p);
}

int op_xor_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_xor_a_8im(gb_emu_p, val);
}

int op_xor_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
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

int op_ld_a_ind_hld(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  z80_p->regs.a = val;
  set_HL(z80_p, (get_HL(z80_p)) - 1);
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
  return op_or_a_8im(gb_emu_p, *reg_p);
}


int op_or_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);

  z80_p->regs.a = z80_p->regs.a | val;
  reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_or_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_or_a_8im(gb_emu_p, val);
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
  uint16_t addr = get_HL(z80_p);
  int val = read_8(gb_emu_p, addr);
  if (val < 0)
    return val;

  uint8_t result = ((uint8_t)val << 4) | ((uint8_t)val >> 4);
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
  uint8_t result = z80_p->regs.a + val;
  
  check_hc_add(z80_p->regs.a, val) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (result < val) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);

  z80_p->regs.a = result;
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
  uint16_t val = read_16(gb_emu_p, sp);
  *reg_pair_p = (qq_code == QQ_PAIR_CODE_AF) ? val & 0xFFF0 : val;
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
  reset_flag_N(z80_p);
  // set H flag if carry from bit 11
  check_hc_add_b16(hl_val, *reg_pair_p) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
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

int op_set_bit_ind_hl(emu *gb_emu_p, uint8_t bit) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  val |= (1UL << bit);
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

  *reg_p = *reg_p + 0x01;
  
  (*reg_p == 0x00) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  reset_flag_N(z80_p);
  return 0;
}

int op_sub_a_r1( emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_sub_a_8im(gb_emu_p, *reg_p);
}

int op_sub_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_sub_a_8im(gb_emu_p, val);

}

int op_sub_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  (z80_p->regs.a & 0x0f) < (val & 0x0f) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (z80_p->regs.a < val) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  z80_p->regs.a = z80_p->regs.a - val;
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_N(z80_p);
  return 0;
}

int op_sbc_a_r1(emu *gb_emu_p, int reg_code){
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_sbc_a_8im(gb_emu_p, *reg_p);
}

int op_sbc_a_8im(emu *gb_emu_p, uint8_t val){
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t result = z80_p->regs.a - val - get_flag_C(z80_p);
  int c = get_flag_C(z80_p);
  (((int)(z80_p->regs.a & 0xf) - (int)(val & 0xf) - c) < 0) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  (((int)(z80_p->regs.a) - (int)val - c) < 0) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_N(z80_p);

  z80_p->regs.a = result;
  return 0;
}

int op_sbc_a_ind_hl(emu *gb_emu_p){
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_sbc_a_8im(gb_emu_p, val);
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

  uint8_t result = (uint8_t)val + 1;
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  check_hc_add((uint8_t)val, 1) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  write_8(gb_emu_p, get_HL(z80_p), result);
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

int op_set_bit_r1(emu *gb_emu_p, uint8_t bit, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  *reg_p |= (0x01 << bit);
  return 0;
}

int op_bit_num_ind_hl(emu *gb_emu_p, uint8_t bit) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  uint8_t b = (val >> bit) & 0x01;
  (b == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  set_flag_H(z80_p);
  reset_flag_N(z80_p);
  return 0;
}

int op_sla_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  (*reg_p & 0x80) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  *reg_p = *reg_p << 1;
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_rlca(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  reset_flag_Z(z80_p);
  uint8_t b = (z80_p->regs.a >> 7) & 0x01;
  (b == 0) ? reset_flag_C(z80_p) : set_flag_C(z80_p);
  z80_p->regs.a = (z80_p->regs.a << 1) | b;
  return 0;
}

int op_rlc_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  uint8_t b = (*reg_p >> 7) & 0x01;
  *reg_p = (*reg_p << 1) | b;

  (b == 0) ? reset_flag_C(z80_p) : set_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_rlc_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  int err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  uint8_t b = (val >> 7) & 0x01;
  uint8_t result = (val << 1) | b;
  (b == 0) ? reset_flag_C(z80_p) : set_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);

  if ((err = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return err;
  return 0;
}

int op_sla_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  int err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  (val & 0x80) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  uint8_t result = val << 1;
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);

  if ((err = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return err;
  return 0;
}

int op_sra_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  int err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  (val & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  uint8_t result = (val >> 1) | (val & 0x80);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);

  if ((err = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return err;
  return 0;
}

int op_rrc_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  int err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;

  (val & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  uint8_t result = (val >> 1) | (val << 7);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);

  if ((err = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return err;
  return 0;
}

int op_adc_a_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  return op_adc_a_8im(gb_emu_p, *reg_p);
}

int op_rr_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  uint8_t val = (*reg_p >> 0x01) | (get_flag_C(z80_p) << 7);
  (*reg_p & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_N(z80_p);
  reset_flag_H(z80_p);
  *reg_p = val;
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_rra(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t val = (z80_p->regs.a >> 0x01) | (get_flag_C(z80_p) << 7);
  (z80_p->regs.a & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_N(z80_p);
  reset_flag_H(z80_p);
  reset_flag_Z(z80_p);
  z80_p->regs.a = val;
  return 0;
}

int op_rr_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  uint8_t result = ((uint8_t)val >> 1) | (get_flag_C(z80_p) << 7);
  (((uint8_t)val) & 1) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_N(z80_p);
  reset_flag_H(z80_p);
  result ? reset_flag_Z(z80_p) : set_flag_Z(z80_p);
  if ((val = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return val;
  return 0;
}

int op_rl_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  int err;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  uint8_t c;
  (val & 0x80) ? (c = 1) : (c = 0);
  uint8_t result = (val << 1) | get_flag_C(z80_p);
  c ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (result == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  if ((err = write_8(gb_emu_p, get_HL(z80_p), result)) < 0)
    return err;
  return 0;
}

int op_adc_a_ind_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  int val;
  if ((val = read_8(gb_emu_p, get_HL(z80_p))) < 0)
    return val;
  return op_adc_a_8im(gb_emu_p, val);
}

int op_adc_a_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t result = z80_p->regs.a + val + get_flag_C(z80_p);
  reset_flag_N(z80_p);
  (((val & 0xf) + (z80_p->regs.a & 0xf) + get_flag_C(z80_p)) > 0xf) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
  ((z80_p->regs.a + val + get_flag_C(z80_p)) > 0xff) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  z80_p->regs.a = result;
  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_ld_ind_16im_sp(emu *gb_emu_p, uint16_t addr) {
  cpu *z80_p = &(gb_emu_p->z80);
  int err;
  if ((err = write_8(gb_emu_p, addr, z80_p->regs.sp_lo)) < 0)
    return err;
  if ((err = write_8(gb_emu_p, addr+1, z80_p->regs.sp_hi)) < 0)
    return err;
  return 0;
}

int op_ldhl_8im(emu *gb_emu_p, uint8_t val){
  cpu *z80_p = &(gb_emu_p->z80);
  int cval = byte_to_2c(val);
  uint16_t abs_cval = (uint16_t)( (cval < 0) ? -cval : cval );
  uint16_t result;
  if (cval < 0) {
    result = get_SP(z80_p) - abs_cval;
    check_hc_sub(get_SP(z80_p), abs_cval) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
    ((result & 0xff) <= (get_SP(z80_p) & 0xff)) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  } else {
    result = get_SP(z80_p) + abs_cval;
    check_hc_add(get_SP(z80_p), abs_cval)? set_flag_H(z80_p) : reset_flag_H(z80_p);
    ((result & 0xFF) <= (get_SP(z80_p) & 0xFF)) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  }
  reset_flag_N(z80_p);
  reset_flag_Z(z80_p);
  z80_p->regs.hl = result;
  return 0;
}

int op_add_sp_8im(emu *gb_emu_p, uint8_t val) {
  cpu *z80_p = &(gb_emu_p->z80);
  int cval = byte_to_2c(val);
  uint16_t abs_cval = (uint16_t)( (cval < 0) ? -cval : cval );
  uint16_t result = get_SP(z80_p) + val;
  if (cval < 0) {
    result = get_SP(z80_p) - abs_cval;
    check_hc_sub(get_SP(z80_p), abs_cval) ? set_flag_H(z80_p) : reset_flag_H(z80_p);
    ((result & 0xff) <= (get_SP(z80_p) & 0xff)) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  } else {
    result = get_SP(z80_p) + abs_cval;
    check_hc_add(get_SP(z80_p), abs_cval)? set_flag_H(z80_p) : reset_flag_H(z80_p);
    ((result & 0xFF) <= (get_SP(z80_p) & 0xFF)) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  }
  reset_flag_N(z80_p);
  reset_flag_Z(z80_p);
  z80_p->regs.sp = result;
  return 0;
}

int op_ld_a_ind_c(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint16_t addr = HW_IO_REGS_START + z80_p->regs.c;
  int val;
  if ((val = read_8(gb_emu_p, addr)) < 0)
    return val;
  z80_p->regs.a = val;
  return 0;
}

int op_scf(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  set_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  return 0;
}

int op_ccf(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  get_flag_C(z80_p) ? reset_flag_C(z80_p) : set_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  return 0;
}

int op_daa(emu *gb_emu_p) {
  // not really sure if any of this is right.
  cpu *z80_p = &(gb_emu_p->z80);

  if (get_flag_N(z80_p) == 0) {
    if ((get_flag_C(z80_p) == 1) || (z80_p->regs.a > 0x99)) { z80_p->regs.a += 0x60; set_flag_C(z80_p); }
    if ((get_flag_H(z80_p) == 1) || ((z80_p->regs.a & 0x0f) > 0x09)) { z80_p->regs.a += 0x06; }
  } else {
    if (get_flag_C(z80_p) == 1) { z80_p->regs.a -= 0x60; }
    if (get_flag_H(z80_p) == 1) { z80_p->regs.a -= 0x6; }
  }

  (z80_p->regs.a == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  reset_flag_H(z80_p);
  return 0;
}

int op_ld_sp_hl(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  set_SP(z80_p, get_HL(z80_p));
  return 0;
}

int op_rla(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t c;
  (z80_p->regs.a & 0x80) ? (c = 1) : (c = 0);
  z80_p->regs.a = (z80_p->regs.a << 1) | get_flag_C(z80_p);
  c ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  reset_flag_Z(z80_p);
  return 0;
}

int op_rl_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  uint8_t c;
  (*reg_p & 0x80) ? (c = 1) : (c = 0);
  *reg_p = (*reg_p << 1) | get_flag_C(z80_p);
  c ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_rrca(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  (z80_p->regs.a & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  z80_p->regs.a = (z80_p->regs.a >> 1) | (z80_p->regs.a << 7);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  reset_flag_Z(z80_p);
  return 0;
}

int op_rrc_r1(emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  (*reg_p & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  *reg_p = (*reg_p >> 1) | (*reg_p << 7);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_sra_r1( emu *gb_emu_p, int reg_code) {
  cpu *z80_p = &(gb_emu_p->z80);
  uint8_t *reg_p;
  int err;
  if ((err = reg_code_to_pointer(z80_p, reg_code, &reg_p)) < 0)
    return err;
  (*reg_p & 0x01) ? set_flag_C(z80_p) : reset_flag_C(z80_p);
  *reg_p = (*reg_p >> 1) | (*reg_p & 0x80);
  reset_flag_H(z80_p);
  reset_flag_N(z80_p);
  (*reg_p == 0) ? set_flag_Z(z80_p) : reset_flag_Z(z80_p);
  return 0;
}

int op_halt(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  z80_p->halt = true;
  return 0;
}

int op_stop(emu *gb_emu_p) {
  //cpu *z80_p = &(gb_emu_p->z80);
  //z80_p->stop = true;
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

  // If EI was our last instruction, we enable the IME after 4 cycles
  if (z80_p->regs.ei_enable) {
    z80_p->regs.ei_cycle_count += z80_p->clk.prev_m_cycles;
    if (z80_p->regs.ei_cycle_count > 4) {
      z80_p->regs.ei_enable = false;
      z80_p->regs.ei_cycle_count = 0;
      set_flag_IME(z80_p);
    }
  }
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
    case (OP_XOR_8IM):
    if ((err = op_xor_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
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
    // EI actually disables interrupts for one instruction, then enables them
    // just before the next instruction is dispatched
    ;
    z80_p->regs.ei_enable = true;
    z80_p->regs.ei_cycle_count = 0;
    reset_flag_IME(z80_p);
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
  case (OP_SET_BIT_REG):
    if ((err = op_set_bit_r1(gb_emu_p, get_bit_offset(op), get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RES_BIT_IND_HL):
    if ((err = op_res_bit_ind_hl(gb_emu_p, get_bit_offset(op))) < 0)
      return err;
    break;
  case (OP_SET_BIT_IND_HL):
    if ((err = op_set_bit_ind_hl(gb_emu_p, get_bit_offset(op))) < 0)
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
  case (OP_RET_NC):
    if (get_flag_C(z80_p) == 0) {
      new_pc = read_16(gb_emu_p, get_SP(z80_p));
      set_SP(z80_p, (get_SP(z80_p) + 2));
    }
    break;
  case (OP_RET_C):
    if (get_flag_C(z80_p) == 1) {
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
  case (OP_SUB_IND_HL):
    if ((err = op_sub_a_ind_hl(gb_emu_p)) < 0)
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
  case (OP_CALL_Z_16IM):
    if (get_flag_Z(z80_p) == 0)
      break;
    sp = get_SP(z80_p);
    if ((err = write_8(gb_emu_p, sp-1, new_pc >> 8)) < 0)
      return err;
    if ((err = write_8(gb_emu_p, sp-2, (new_pc & 0x00FF))) < 0)
      return err;
    new_pc = read_16(gb_emu_p, pc+1);
    set_SP(z80_p, sp - 2);
    break;
  case (OP_CALL_NC_16IM):
    if (get_flag_C(z80_p) != 0)
      break;
    sp = get_SP(z80_p);
    if ((err = write_8(gb_emu_p, sp-1, new_pc >> 8)) < 0)
      return err;
    if ((err = write_8(gb_emu_p, sp-2, (new_pc & 0x00FF))) < 0)
      return err;
    new_pc = read_16(gb_emu_p, pc+1);
    set_SP(z80_p, sp - 2);
    break;
  case (OP_CALL_C_16IM):
    if (get_flag_C(z80_p) == 0)
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
  case (OP_SLA_REG):
    if ((err = op_sla_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
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
  case (OP_BIT_NUM_IND_HL):
    if ((err = op_bit_num_ind_hl(gb_emu_p, get_bit_offset(op))) < 0)
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
  case (OP_JP_NC_16IM):
    if (get_flag_C(z80_p) == 0)
      new_pc = read_16(gb_emu_p, pc+1);
    break;
  case (OP_LD_A_IND_HLD):
    if ((err = op_ld_a_ind_hld(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_OR_8IM):
    if ((err = op_or_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_OR_IND_HL):
    if ((err = op_or_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RLCA):
    if ((err = op_rlca(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_ADC_A_REG):
    if ((err = op_adc_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_ADC_A_IND_HL):
    if ((err = op_adc_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_ADC_A_8IM):
    if ((err = op_adc_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_RR_REG):
    if ((err = op_rr_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RR_IND_HL):
    if ((err = op_rr_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_DAA):
    if ((err = op_daa(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RRA):
    if ((err = op_rra(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_IND_16IM_SP):
    if ((err = op_ld_ind_16im_sp(gb_emu_p, read_16(gb_emu_p, pc+1))) < 0)
      return err;
    break;
   case (OP_LD_SP_HL):
    if ((err = op_ld_sp_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_ADD_SP_8IM):
    if ((err = op_add_sp_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_LD_HL_8IM):
    if ((err = op_ldhl_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_SBC_8IM):
    if ((err = op_sbc_a_8im(gb_emu_p, read_8(gb_emu_p, pc+1))) < 0)
      return err;
    break;
  case (OP_SBC_REG):
    if ((err = op_sbc_a_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_SBC_IND_HL):
    if ((err = op_sbc_a_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_LD_A_IND_C):
    if ((err = op_ld_a_ind_c(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_SCF):
    if ((err = op_scf(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_CCF):
    if ((err = op_ccf(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RLA):
    if ((err = op_rla(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RRCA):
    if ((err = op_rrca(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RLC_REG):
    if ((err = op_rlc_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RLC_IND_HL):
    if ((err = op_rlc_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RRC_REG):
    if ((err = op_rrc_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RRC_IND_HL):
    if ((err = op_rrc_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_RL_REG):
    if ((err = op_rl_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_RL_IND_HL):
    if ((err = op_rl_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_SRA_REG):
    if ((err = op_sra_r1(gb_emu_p, get_reg_code_lo(op))) < 0)
      return err;
    break;
  case (OP_SRA_IND_HL):
    if ((err = op_sra_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_SLA_IND_HL):
    if ((err = op_sla_ind_hl(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_HALT):
    if ((err = op_halt(gb_emu_p)) < 0)
      return err;
    break;
  case (OP_STOP):
    if ((err = op_stop(gb_emu_p)) < 0)
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
  return (((a & 0xf) + (b & 0xf)) & 0x10) == 0x10;

}

bool check_hc_add_b16(uint16_t a, uint16_t b) {
  return (((a & 0xfff) + (b & 0xfff)) >= 0x1000);
}

bool check_hc_sub(uint8_t a, uint8_t b) {
  return ((a - b) & 0xf) <= (a & 0xf);
}

bool check_hc_sub_b16(uint16_t a, uint16_t b) {
  return ((a & 0xfff) < (b & 0xfff));
}


