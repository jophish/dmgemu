#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include <stdio.h>
#include "mmu.h"
#include "emu.h"
#include "cpu.h"
#include "rom.h"
#include "util.h"

#define CB_MASK 0xCB
#define LSB_MASK 0x00FF
#define MAX_MNEMONIC_LEN 0x10


enum reg_code {
  REG_CODE_A = 0b111,
  REG_CODE_B = 0b000,
  REG_CODE_C = 0b001,
  REG_CODE_D = 0b010,
  REG_CODE_E = 0b011,
  REG_CODE_H = 0b100,
  REG_CODE_L = 0b101,
};

enum dd_pair_code {
  DD_PAIR_CODE_BC = 0b00,
  DD_PAIR_CODE_DE = 0b01,
  DD_PAIR_CODE_HL = 0b10,
  DD_PAIR_CODE_SP = 0b11,
};

enum ss_pair_code {
  SS_PAIR_CODE_BC = 0b00,
  SS_PAIR_CODE_DE = 0b01,
  SS_PAIR_CODE_HL = 0b10,
  SS_PAIR_CODE_SP = 0b11,
};

enum qq_pair_code {
  QQ_PAIR_CODE_BC = 0b00,
  QQ_PAIR_CODE_DE = 0b01,
  QQ_PAIR_CODE_HL = 0b10,
  QQ_PAIR_CODE_AF = 0b11,
};

enum t_pair_code {
  T_CODE_00 = 0b000,
  T_CODE_08 = 0b001,
  T_CODE_10 = 0b010,
  T_CODE_18 = 0b011,
  T_CODE_20 = 0b100,
  T_CODE_28 = 0b101,
  T_CODE_30 = 0b110,
  T_CODE_38 = 0b111,
};

enum arg_enum {
  ARG_NONE,
  ARG_BC,
  ARG_D16,      // next two bytes
  ARG_IND_BC,
  ARG_A,
  ARG_B,
  ARG_C,
  ARG_D,
  ARG_E,
  ARG_H,
  ARG_Z,
  ARG_L,
  ARG_38H,
  ARG_SPIR8,
  ARG_30H,
  ARG_AF,
  ARG_28H,
  ARG_20H,
  ARG_IND_C,
  ARG_IND_A8,
  ARG_18H,
  ARG_10H,
  ARG_08H,
  ARG_CB,
  ARG_00H,
  ARG_A8,
  ARG_A16,
  ARG_IND_HL,
  ARG_IND_HLD,
  ARG_NC,
  ARG_IND_HLI,
  ARG_NZ,
  ARG_R8,
  ARG_IND_DE,
  ARG_DE,
  ARG_0,
  ARG_HL,
  ARG_SP,
  ARG_IND_A16,
  ARG_1,
  ARG_2,
  ARG_3,
  ARG_4,
  ARG_5,
  ARG_6,
  ARG_7,
  ARG_D8,
};

enum op_type {
  OP_NOP,
  OP_LD_REG_8IM,
  OP_LD_DD_16IM,
  OP_JP_16IM,
  OP_XOR_REG,
  OP_LD_IND_HLD_A,
  OP_DEC_REG,
  OP_JR_NZ_8IM,
  OP_CP_A_REG,
  OP_CP_A_IND_HL,
  OP_CP_A_8IM,
  OP_DI,
  OP_LDH_8IM_A,
  OP_LDH_A_8IM,
  OP_LD_IND_HL_8IM,
  OP_LD_IND_16IM_A,
  OP_LD_A_IND_HLI,
  OP_DEC_SS,
  OP_LD_IND_C_A,
  OP_CALL_16IM,
  OP_LD_R1_R2,
  OP_RET,
  OP_OR_REG,
  OP_EI,
  OP_CPL,
  OP_AND_A_8IM,
  OP_AND_A_REG,
  OP_AND_A_IND_HL,
  OP_SWAP_REG,
  OP_SWAP_IND_HL,
  OP_ADD_A_REG,
  OP_ADD_A_IND_HL,
  OP_ADD_A_8IM,
  OP_RST,
  OP_POP,
  OP_ADD_HL_SS,
  OP_LD_REG_IND_HL,
  OP_INC_SS,
  OP_PUSH,
  OP_JP_IND_HL,
  OP_RES_BIT_REG,
  OP_RES_BIT_IND_HL,
  OP_LD_IND_DE_A,
  OP_LD_IND_BC_A,
  OP_LD_A_IND_DE,
  OP_LD_A_IND_BC,
  OP_LD_IND_HLI_A,
  OP_LD_A_IND_IM16,
  OP_JR_Z_8IM,
  OP_INC_REG,
  OP_JP_Z_16IM,
  OP_JP_NZ_16IM,
  OP_RET_Z,
  OP_JR_8IM,
  OP_SUB_REG,
  OP_DEC_IND_HL,
  OP_RETI,
  OP_LD_IND_HL_REG,
  OP_RET_NZ,
  OP_INC_IND_HL,
  OP_CALL_NZ_16IM,
  OP_SUB_8IM,
  OP_XOR_IND_HL,
  OP_SRL_REG,
  OP_SRL_IND_HL,
  OP_BIT_NUM_REG,
  OP_JR_NC_8IM,
  OP_JR_C_8IM,
  OP_JP_C_16IM,
  OP_SLA_REG,
  OP_LD_A_IND_HLD,
  OP_OR_8IM,
  OP_BIT_NUM_IND_HL,
  OP_RLCA,
  OP_ADC_A_REG,
  OP_DAA,
  OP_ADC_A_IND_HL,
  OP_ADC_A_8IM,
  OP_RET_NC,
  OP_RET_C,
  OP_XOR_8IM,
  OP_SET_BIT_IND_HL,
  OP_SET_BIT_REG,
  OP_SUB_IND_HL,
  OP_TMP,
  OP_JP_NC_16IM,
  OP_RR_REG,
  OP_RR_IND_HL,
  OP_RRA,
  OP_OR_IND_HL,
  OP_LD_IND_16IM_SP,
  OP_LD_SP_HL,
  OP_LD_HL_8IM,
  OP_ADD_SP_8IM,
  OP_SBC_8IM,
  OP_CALL_Z_16IM,
  OP_CALL_NC_16IM,
  OP_CALL_C_16IM,
  OP_LD_A_IND_C,
};

typedef struct opcode {
  uint8_t op, cyc1, cyc2, len;      // opcode, normal cycles, conditional cycles, len
  bool extended;                    // 0xCB prefix?
  int arg1, arg2;                   // enums for arguments -- used for debugging purposes
  char mnemonic[MAX_MNEMONIC_LEN];  // for debugging
  int op_type;                     // enum for switching over in op dispatcherx
} opcode;

// Pointers to two registers for opcodes that deal with register pairs
typedef struct reg_pair {
  uint8_t *r1, *r2;
} reg_pair;


/**********************/
/* Opcode Dispatchers */
/**********************/

// 8 Bit Transfer
int op_ld_r1_r2(emu *gb_emu_p, int reg_code1, int reg_code2);
int op_ld_r_8im(emu *gb_emu_p, int reg_code, uint8_t val);
int op_ld_r_ind_hl(emu *gb_emu_p, int reg_code);
int op_ld_ind_hl_r(emu *gb_emu_p, int reg_code);
int op_ld_ind_hl_8im(emu *gb_emu_p, uint8_t val);
int op_ld_a_ind_bc(emu *gb_emu_p);
int op_ld_a_ind_de(emu *gb_emu_p);
int op_ld_a_ind_c(emu *gb_emu_p);
int op_ld_ind_c_a(emu *gb_emu_p);
int op_ld_a_ind_8im(emu *gb_emu_p, uint8_t val);
int op_ld_ind_8im_a(emu *gb_emu_p, uint8_t addr);
int op_ld_a_ind_16im(emu *gb_emu_p, uint16_t addr);
int op_ld_ind_16im_a(emu *gb_emu_p, uint16_t addr);
int op_ld_a_ind_hli(emu *gb_emu_p);
int op_ld_a_ind_hld(emu *gb_emu_p);
int op_ld_ind_bc_a(emu *gb_emu_p);
int op_ld_ind_de_a(emu *gb_emu_p);
int op_ld_ind_hli_a(emu *gb_emu_p);
int op_ld_ind_hld_a(emu *gb_emu_p);

// 16 Bit Transfer
int op_ld_dd_16im(emu *gb_emu_p, int dd_code, uint16_t addr);
int op_ld_sp_hl(emu *gb_emu_p);
int op_push_qq(emu *gb_emu_p, int qq_code);
int op_pop_qq(emu *gb_emu_p, int qq_code);
int op_ldhl_8im(emu *gb_emu_p, uint8_t val);
int op_ld_ind_16im_sp(emu *gb_emu_p, uint16_t addr);

// 8 Bit Arithmetic and Logical Operations
int op_add_a_r1(emu *gb_emu_p, int reg_code);
int op_add_a_8im(emu *gb_emu_p, uint8_t val);
int op_add_a_ind_hl(emu *gb_emu_p);
int op_adc_a_r1(emu *gb_emu_p, int reg_code);
int op_adc_a_8im(emu *gb_emu_p, uint8_t val);
int op_adc_a_ind_hl(emu *gb_emu_p);
int op_sub_a_r1(emu *gb_emu_p, int reg_code);
int op_sub_a_8im(emu *gb_emu_p, uint8_t val);
int op_sub_a_ind_hl(emu *gb_emu_p);
int op_sbc_a_r1(emu *gb_emu_p, int reg_code);
int op_sbc_a_8im(emu *gb_emu_p, uint8_t val);
int op_sbc_a_ind_hl(emu *gb_emu_p);
int op_and_a_r1(emu *gb_emu_p, int reg_code);
int op_and_a_8im(emu *gb_emu_p, uint8_t val);
int op_and_a_ind_hl(emu *gb_emu_p);
int op_or_a_r1(emu *gb_emu_p, int reg_code);
int op_or_a_8im(emu *gb_emu_p, uint8_t val);
int op_or_a_ind_hl(emu *gb_emu_p);
int op_xor_a_r1(emu *gb_emu_p, int reg_code);
int op_xor_a_8im(emu *gb_emu_p, uint8_t val);
int op_xor_a_ind_hl(emu *gb_emu_p);
int op_cp_a_r1(emu *gb_emu_p, int reg_code);
int op_cp_a_8im(emu *gb_emu_p, uint8_t val);
int op_cp_a_ind_hl(emu *gb_emu_p);
int op_inc_r1(emu *gb_emu_p, int reg_code);
int op_inc_ind_hl(emu *gb_emu_p);
int op_dec_r1(emu *gb_emu_p, int reg_code);
int op_dec_ind_hl(emu *gb_emu_p);

// 16 Bit Arithmetic Operation Instructions
int op_add_hl_ss(emu *gb_emu_p, int ss_code);
int op_add_sp_8im(emu *gb_emu_p, uint8_t val);
int op_inc_ss(emu *gb_emu_p, int ss_code);
int op_dec_ss(emu *gb_emu_p, int ss_code);


// Call and Return Instructions
int op_cpl(emu *gb_emu_p);

// Bit Operations
int op_bit_num_r1(emu *gb_emu_p, uint8_t bit, int reg_code);
int op_bit_num_ind_hl(emu *gb_emu_p, uint8_t bit);
int op_res_bit_r1(emu *gb_emu_p, uint8_t bit, int reg_code);
int op_res_bit_ind_hl(emu *gb_emu_p, uint8_t bit);
int op_sla_r1(emu *gb_emu_p, int reg_code);
int op_rlca(emu *gb_emu_p);
int op_set_bit_ind_hl(emu *gb_emu_p, uint8_t bit);
int op_set_bit_r1(emu *gb_emu_p, uint8_t bit, int reg_code);
int op_rr_r1(emu *gb_emu_p, int reg_code);
int op_rr_ind_hl(emu *gb_emu_p);
int op_rra(emu *gb_emu_p);

// CB Instructions
int op_swap_r1(emu *gb_emu_p, int reg_code);
int op_swap_ind_hl(emu *gb_emu_p);

// Rotate Shift Instructions
int op_srl_r1(emu *gb_emu_p, int reg_code);
int op_srl_ind_hl(emu *gb_emu_p);

int op_daa(emu *gb_emu_p);


// Given an argument, determines whether or not the mnemonic requires
// arguments based off of bytes following the opcode.
bool special_arg(uint8_t arg);

// Given a dd code, fills reg_pair_p with pointers to the appropriate
// registers.
int dd_to_reg_pair(cpu *z80_p, int dd_code, reg_pair *reg_pair_p);

// Given a qq code, fills reg_pair_pp with a pointer to the correct 16 bit register pair
int qq_to_reg_pair(cpu *z80_p, int qq_code, uint16_t **reg_pair_pp);

// Given an ss code, fills reg_pair_pp with a pointer to the correct 16 bit register pair
int ss_to_reg_pair(cpu *z80_p, int ss_code, uint16_t **reg_pair_pp);

// Given a reg code, fills reg_p with a pointer to the appropriate
// register.
int reg_code_to_pointer(cpu *z80_p, int reg_code, uint8_t **reg_p);

// Given a cpu and a rom, will execute the instruction in the rom
// located at the current PC in the cpu. Returns the opcode of the
// instruction executed. If the opcode was a 0xCB prefixed op, then the
// MSB of the return value will be 0xCB, otherwise, 0.
int dispatch_op(emu *gb_emu_p, opcode *op_p);

// Given a memory address, interprets it as an instruction and
// fills the buf with the string mnemonic of the opcode including
// any immediate data that might follow the code. Returns ERR_OP_INVALID_OR_NOT_IMPLEMENTED
// if the memory at addr does not correspond to a known instruction. Returns ERR_BUF_LEN
// if the given buffer is too small for the given string. If op_struct_p is given,
// the struct for the specified opcode will be returned in it.
int addr_to_op_str(emu *gb_emu_p, uint16_t addr, char *buf, int buf_len, opcode *op_struct_p);

// Given an address, fills in the given op_struct_p according to which op
// is located at the address. Returns true if the given op was a CB prefixed op,
// otherwise returns false.
bool addr_to_op(emu *gb_emu_p, uint16_t addr, opcode *op_struct_p);

// Given bytes a and b to add together, checks if a half-carry will occur
bool check_hc_add(uint8_t a, uint8_t b);
bool check_hc_add_b16(uint16_t a, uint16_t b);

bool check_hc_sub(uint8_t a, uint8_t b);
bool check_hc_sub_b16(uint16_t a, uint16_t b);



// Masks
#define BYTE_MASK 0xFF
#define NIBBLE_MASK 0xF

static const opcode op_array[256] = {
{0x0, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "nop", OP_NOP},
{0x1, 12, 0, 3,  false, ARG_BC, ARG_D16, "ld bc, 0x%04x", OP_LD_DD_16IM},
{0x2, 8, 0, 1,  false, ARG_IND_BC, ARG_A, "ld (bc), a", OP_LD_IND_BC_A},
{0x3, 8, 0, 1,  false, ARG_BC, ARG_NONE, "inc bc", OP_INC_SS},
{0x4, 4, 0, 1,  false, ARG_B, ARG_NONE, "inc b", OP_INC_REG},
{0x5, 4, 0, 1,  false, ARG_B, ARG_NONE, "dec b", OP_DEC_REG},
{0x6, 8, 0, 2,  false, ARG_B, ARG_D8, "ld b, 0x%02x", OP_LD_REG_8IM},
{0x7, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rlca", OP_RLCA},
{0x8, 20, 0, 3,  false, ARG_IND_A16, ARG_SP, "ld (0x%04x), sp", OP_LD_IND_16IM_SP},
{0x9, 8, 0, 1,  false, ARG_HL, ARG_BC, "add hl, bl", OP_ADD_HL_SS},
{0xa, 8, 0, 1,  false, ARG_A, ARG_IND_BC, "ld a, (bc)", OP_LD_A_IND_BC},
{0xb, 8, 0, 1,  false, ARG_BC, ARG_NONE, "dec bc", OP_DEC_SS},
{0xc, 4, 0, 1,  false, ARG_C, ARG_NONE, "inc c", OP_INC_REG},
{0xd, 4, 0, 1,  false, ARG_C, ARG_NONE, "dec c", OP_DEC_REG},
{0xe, 8, 0, 2,  false, ARG_C, ARG_D8, "ld c, 0x%02x", OP_LD_REG_8IM},
{0xf, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rrca", OP_TMP},
{0x10, 4, 0, 2,  false, ARG_0, ARG_NONE, "stop 0", OP_TMP},
{0x11, 12, 0, 3,  false, ARG_DE, ARG_D16, "ld de, 0x%04x", OP_LD_DD_16IM},
{0x12, 8, 0, 1,  false, ARG_IND_DE, ARG_A, "ld (de), a", OP_LD_IND_DE_A},
{0x13, 8, 0, 1,  false, ARG_DE, ARG_NONE, "inc de", OP_INC_SS},
{0x14, 4, 0, 1,  false, ARG_D, ARG_NONE, "inc d", OP_INC_REG},
{0x15, 4, 0, 1,  false, ARG_D, ARG_NONE, "dec d", OP_DEC_REG},
{0x16, 8, 0, 2,  false, ARG_D, ARG_D8, "ld d, 0x%02x", OP_LD_REG_8IM},
{0x17, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rla", OP_TMP},
{0x18, 12, 0, 2,  false, ARG_R8, ARG_NONE, "jr 0x%04x", OP_JR_8IM},
{0x19, 8, 0, 1,  false, ARG_HL, ARG_DE, "add hl, de", OP_ADD_HL_SS},
{0x1a, 8, 0, 1,  false, ARG_A, ARG_IND_DE, "ld a, (de)", OP_LD_A_IND_DE},
{0x1b, 8, 0, 1,  false, ARG_DE, ARG_NONE, "dec de", OP_DEC_SS},
{0x1c, 4, 0, 1,  false, ARG_E, ARG_NONE, "inc e", OP_INC_REG},
{0x1d, 4, 0, 1,  false, ARG_E, ARG_NONE, "dec e", OP_DEC_REG},
{0x1e, 8, 0, 2,  false, ARG_E, ARG_D8, "ld e, 0x%02x", OP_LD_REG_8IM},
{0x1f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rra", OP_RRA},
{0x20, 12, 8, 2,  false, ARG_NZ, ARG_R8, "jr nz, 0x%04x", OP_JR_NZ_8IM},
{0x21, 12, 0, 3,  false, ARG_HL, ARG_D16, "ld hl, 0x%04x", OP_LD_DD_16IM},
{0x22, 8, 0, 1,  false, ARG_IND_HLI, ARG_A, "ld (hli), a", OP_LD_IND_HLI_A},
{0x23, 8, 0, 1,  false, ARG_HL, ARG_NONE, "inc hl", OP_INC_SS},
{0x24, 4, 0, 1,  false, ARG_H, ARG_NONE, "inc h", OP_INC_REG},
{0x25, 4, 0, 1,  false, ARG_H, ARG_NONE, "dec h", OP_DEC_REG},
{0x26, 8, 0, 2,  false, ARG_H, ARG_D8, "ld h, 0x%02x", OP_LD_REG_8IM},
{0x27, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "daa", OP_DAA},
{0x28, 12, 8, 2,  false, ARG_Z, ARG_R8, "jr z, 0x%04x", OP_JR_Z_8IM},
{0x29, 8, 0, 1,  false, ARG_HL, ARG_HL, "add hl, hl", OP_ADD_HL_SS},
{0x2a, 8, 0, 1,  false, ARG_A, ARG_IND_HLI, "ld a, (hli)", OP_LD_A_IND_HLI},
{0x2b, 8, 0, 1,  false, ARG_HL, ARG_NONE, "dec hl", OP_DEC_SS},
{0x2c, 4, 0, 1,  false, ARG_L, ARG_NONE, "inc l", OP_INC_REG},
{0x2d, 4, 0, 1,  false, ARG_L, ARG_NONE, "dec l", OP_DEC_REG},
{0x2e, 8, 0, 2,  false, ARG_L, ARG_D8, "ld l, 0x%02x", OP_LD_REG_8IM},
{0x2f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "cpl", OP_CPL},
{0x30, 12, 8, 2,  false, ARG_NC, ARG_R8, "jr nc, 0x%04x", OP_JR_NC_8IM},
{0x31, 12, 0, 3,  false, ARG_SP, ARG_D16, "ld sp, 0x%04x", OP_LD_DD_16IM},
{0x32, 8, 0, 1,  false, ARG_IND_HLD, ARG_A, "ld (hld), a", OP_LD_IND_HLD_A},
{0x33, 8, 0, 1,  false, ARG_SP, ARG_NONE, "inc sp", OP_INC_SS},
{0x34, 12, 0, 1,  false, ARG_IND_HL, ARG_NONE, "inc (hl)", OP_INC_IND_HL},
{0x35, 12, 0, 1,  false, ARG_IND_HL, ARG_NONE, "dec (hl)", OP_DEC_IND_HL},
{0x36, 12, 0, 2,  false, ARG_IND_HL, ARG_D8, "ld (hl), 0x%02x", OP_LD_IND_HL_8IM},
{0x37, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "scf", OP_TMP},
{0x38, 12, 8, 2,  false, ARG_C, ARG_R8, "jr c, 0x%04x", OP_JR_C_8IM},
{0x39, 8, 0, 1,  false, ARG_HL, ARG_SP, "add hl, sp", OP_ADD_HL_SS},
{0x3a, 8, 0, 1,  false, ARG_A, ARG_IND_HLD, "ld a, (hld)", OP_LD_A_IND_HLD},
{0x3b, 8, 0, 1,  false, ARG_SP, ARG_NONE, "dec sp", OP_DEC_SS},
{0x3c, 4, 0, 1,  false, ARG_A, ARG_NONE, "inc a", OP_INC_REG},
{0x3d, 4, 0, 1,  false, ARG_A, ARG_NONE, "dec a", OP_DEC_REG},
{0x3e, 8, 0, 2,  false, ARG_A, ARG_D8, "ld a, 0x%02x", OP_LD_REG_8IM},
{0x3f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "ccf", OP_TMP},
{0x40, 4, 0, 1,  false, ARG_B, ARG_B, "ld b, b", OP_LD_R1_R2},
{0x41, 4, 0, 1,  false, ARG_B, ARG_C, "ld b, c", OP_LD_R1_R2},
{0x42, 4, 0, 1,  false, ARG_B, ARG_D, "ld b, d", OP_LD_R1_R2},
{0x43, 4, 0, 1,  false, ARG_B, ARG_E, "ld b, e", OP_LD_R1_R2},
{0x44, 4, 0, 1,  false, ARG_B, ARG_H, "ld b, h", OP_LD_R1_R2},
{0x45, 4, 0, 1,  false, ARG_B, ARG_L, "ld b, l", OP_LD_R1_R2},
{0x46, 8, 0, 1,  false, ARG_B, ARG_IND_HL, "ld b, (hl)", OP_LD_REG_IND_HL},
{0x47, 4, 0, 1,  false, ARG_B, ARG_A, "ld b, a", OP_LD_R1_R2},
{0x48, 4, 0, 1,  false, ARG_C, ARG_B, "ld c, b", OP_LD_R1_R2},
{0x49, 4, 0, 1,  false, ARG_C, ARG_C, "ld c, c", OP_LD_R1_R2},
{0x4a, 4, 0, 1,  false, ARG_C, ARG_D, "ld c, d", OP_LD_R1_R2},
{0x4b, 4, 0, 1,  false, ARG_C, ARG_E, "ld c, e", OP_LD_R1_R2},
{0x4c, 4, 0, 1,  false, ARG_C, ARG_H, "ld c, h", OP_LD_R1_R2},
{0x4d, 4, 0, 1,  false, ARG_C, ARG_L, "ld c, l", OP_LD_R1_R2},
{0x4e, 8, 0, 1,  false, ARG_C, ARG_IND_HL, "ld c, (hl)", OP_LD_REG_IND_HL},
{0x4f, 4, 0, 1,  false, ARG_C, ARG_A, "ld c, a", OP_LD_R1_R2},
{0x50, 4, 0, 1,  false, ARG_D, ARG_B, "ld d, b", OP_LD_R1_R2},
{0x51, 4, 0, 1,  false, ARG_D, ARG_C, "ld d, c", OP_LD_R1_R2},
{0x52, 4, 0, 1,  false, ARG_D, ARG_D, "ld d, d", OP_LD_R1_R2},
{0x53, 4, 0, 1,  false, ARG_D, ARG_E, "ld d, e", OP_LD_R1_R2},
{0x54, 4, 0, 1,  false, ARG_D, ARG_H, "ld d, h", OP_LD_R1_R2},
{0x55, 4, 0, 1,  false, ARG_D, ARG_L, "ld d, l", OP_LD_R1_R2},
{0x56, 8, 0, 1,  false, ARG_D, ARG_IND_HL, "ld d, (hl)", OP_LD_REG_IND_HL},
{0x57, 4, 0, 1,  false, ARG_D, ARG_A, "ld d, a", OP_LD_R1_R2},
{0x58, 4, 0, 1,  false, ARG_E, ARG_B, "ld e, b", OP_LD_R1_R2},
{0x59, 4, 0, 1,  false, ARG_E, ARG_C, "ld e, c", OP_LD_R1_R2},
{0x5a, 4, 0, 1,  false, ARG_E, ARG_D, "ld e, d", OP_LD_R1_R2},
{0x5b, 4, 0, 1,  false, ARG_E, ARG_E, "ld e, e", OP_LD_R1_R2},
{0x5c, 4, 0, 1,  false, ARG_E, ARG_H, "ld e, h", OP_LD_R1_R2},
{0x5d, 4, 0, 1,  false, ARG_E, ARG_L, "ld e, l", OP_LD_R1_R2},
{0x5e, 8, 0, 1,  false, ARG_E, ARG_IND_HL, "ld e, (hl)", OP_LD_REG_IND_HL},
{0x5f, 4, 0, 1,  false, ARG_E, ARG_A, "ld e, a", OP_LD_R1_R2},
{0x60, 4, 0, 1,  false, ARG_H, ARG_B, "ld h, b", OP_LD_R1_R2},
{0x61, 4, 0, 1,  false, ARG_H, ARG_C, "ld h, c", OP_LD_R1_R2},
{0x62, 4, 0, 1,  false, ARG_H, ARG_D, "ld h, d", OP_LD_R1_R2},
{0x63, 4, 0, 1,  false, ARG_H, ARG_E, "ld h, e", OP_LD_R1_R2},
{0x64, 4, 0, 1,  false, ARG_H, ARG_H, "ld h, h", OP_LD_R1_R2},
{0x65, 4, 0, 1,  false, ARG_H, ARG_L, "ld h, l", OP_LD_R1_R2},
{0x66, 8, 0, 1,  false, ARG_H, ARG_IND_HL, "ld h, (hl)", OP_LD_REG_IND_HL},
{0x67, 4, 0, 1,  false, ARG_H, ARG_A, "ld h, a", OP_LD_R1_R2},
{0x68, 4, 0, 1,  false, ARG_L, ARG_B, "ld l, b", OP_LD_R1_R2},
{0x69, 4, 0, 1,  false, ARG_L, ARG_C, "ld l, c", OP_LD_R1_R2},
{0x6a, 4, 0, 1,  false, ARG_L, ARG_D, "ld l, d", OP_LD_R1_R2},
{0x6b, 4, 0, 1,  false, ARG_L, ARG_E, "ld l, e", OP_LD_R1_R2},
{0x6c, 4, 0, 1,  false, ARG_L, ARG_H, "ld l, h", OP_LD_R1_R2},
{0x6d, 4, 0, 1,  false, ARG_L, ARG_L, "ld l, l", OP_LD_R1_R2},
{0x6e, 8, 0, 1,  false, ARG_L, ARG_IND_HL, "ld l, (hl)", OP_LD_REG_IND_HL},
{0x6f, 4, 0, 1,  false, ARG_L, ARG_A, "ld l, a", OP_LD_R1_R2},
{0x70, 8, 0, 1,  false, ARG_IND_HL, ARG_B, "ld (hl), b", OP_LD_IND_HL_REG},
{0x71, 8, 0, 1,  false, ARG_IND_HL, ARG_C, "ld (hl), c", OP_LD_IND_HL_REG},
{0x72, 8, 0, 1,  false, ARG_IND_HL, ARG_D, "ld (hl), d", OP_LD_IND_HL_REG},
{0x73, 8, 0, 1,  false, ARG_IND_HL, ARG_E, "ld (hl), e", OP_LD_IND_HL_REG},
{0x74, 8, 0, 1,  false, ARG_IND_HL, ARG_H, "ld (hl), h", OP_LD_IND_HL_REG},
{0x75, 8, 0, 1,  false, ARG_IND_HL, ARG_L, "ld (hl), l", OP_LD_IND_HL_REG},
{0x76, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "halt", OP_TMP},
{0x77, 8, 0, 1,  false, ARG_IND_HL, ARG_A, "ld (hl), a", OP_LD_IND_HL_REG},
{0x78, 4, 0, 1,  false, ARG_A, ARG_B, "ld a, b", OP_LD_R1_R2},
{0x79, 4, 0, 1,  false, ARG_A, ARG_C, "ld a, c", OP_LD_R1_R2},
{0x7a, 4, 0, 1,  false, ARG_A, ARG_D, "ld a, d", OP_LD_R1_R2},
{0x7b, 4, 0, 1,  false, ARG_A, ARG_E, "ld a, e", OP_LD_R1_R2},
{0x7c, 4, 0, 1,  false, ARG_A, ARG_H, "ld a, h", OP_LD_R1_R2},
{0x7d, 4, 0, 1,  false, ARG_A, ARG_L, "ld a, l", OP_LD_R1_R2},
{0x7e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "ld a, (hl)", OP_LD_REG_IND_HL},
{0x7f, 4, 0, 1,  false, ARG_A, ARG_A, "ld a, a", OP_LD_R1_R2},
{0x80, 4, 0, 1,  false, ARG_A, ARG_B, "add a, b", OP_ADD_A_REG},
{0x81, 4, 0, 1,  false, ARG_A, ARG_C, "add a, c", OP_ADD_A_REG},
{0x82, 4, 0, 1,  false, ARG_A, ARG_D, "add a, d", OP_ADD_A_REG},
{0x83, 4, 0, 1,  false, ARG_A, ARG_E, "add a, e", OP_ADD_A_REG},
{0x84, 4, 0, 1,  false, ARG_A, ARG_H, "add a, h", OP_ADD_A_REG},
{0x85, 4, 0, 1,  false, ARG_A, ARG_L, "add a, l", OP_ADD_A_REG},
{0x86, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "add a, (hl)", OP_ADD_A_IND_HL},
{0x87, 4, 0, 1,  false, ARG_A, ARG_A, "add a, a", OP_ADD_A_REG},
{0x88, 4, 0, 1,  false, ARG_A, ARG_B, "adc a, b", OP_ADC_A_REG},
{0x89, 4, 0, 1,  false, ARG_A, ARG_C, "adc a, c", OP_ADC_A_REG},
{0x8a, 4, 0, 1,  false, ARG_A, ARG_D, "adc a, d", OP_ADC_A_REG},
{0x8b, 4, 0, 1,  false, ARG_A, ARG_E, "adc a, e", OP_ADC_A_REG},
{0x8c, 4, 0, 1,  false, ARG_A, ARG_H, "adc a, h", OP_ADC_A_REG},
{0x8d, 4, 0, 1,  false, ARG_A, ARG_L, "adc a, l", OP_ADC_A_REG},
{0x8e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "adc a, (hl)", OP_ADC_A_IND_HL},
{0x8f, 4, 0, 1,  false, ARG_A, ARG_A, "adc a, a", OP_ADC_A_REG},
{0x90, 4, 0, 1,  false, ARG_B, ARG_NONE, "sub b", OP_SUB_REG},
{0x91, 4, 0, 1,  false, ARG_C, ARG_NONE, "sub c", OP_SUB_REG},
{0x92, 4, 0, 1,  false, ARG_D, ARG_NONE, "sub d", OP_SUB_REG},
{0x93, 4, 0, 1,  false, ARG_E, ARG_NONE, "sub e", OP_SUB_REG},
{0x94, 4, 0, 1,  false, ARG_H, ARG_NONE, "sub h", OP_SUB_REG},
{0x95, 4, 0, 1,  false, ARG_L, ARG_NONE, "sub l", OP_SUB_REG},
{0x96, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "sub (hl)", OP_SUB_IND_HL},
{0x97, 4, 0, 1,  false, ARG_A, ARG_NONE, "sub a", OP_SUB_REG},
{0x98, 4, 0, 1,  false, ARG_A, ARG_B, "sbc a, b", OP_TMP},
{0x99, 4, 0, 1,  false, ARG_A, ARG_C, "sbc a, c", OP_TMP},
{0x9a, 4, 0, 1,  false, ARG_A, ARG_D, "sbc a, d", OP_TMP},
{0x9b, 4, 0, 1,  false, ARG_A, ARG_E, "sbc a, e", OP_TMP},
{0x9c, 4, 0, 1,  false, ARG_A, ARG_H, "sbc a, h", OP_TMP},
{0x9d, 4, 0, 1,  false, ARG_A, ARG_L, "sbc a, l", OP_TMP},
{0x9e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "sbc a, (hl)", OP_TMP},
{0x9f, 4, 0, 1,  false, ARG_A, ARG_A, "sbc a, a", OP_TMP},
{0xa0, 4, 0, 1,  false, ARG_B, ARG_NONE, "and b", OP_AND_A_REG},
{0xa1, 4, 0, 1,  false, ARG_C, ARG_NONE, "and c", OP_AND_A_REG},
{0xa2, 4, 0, 1,  false, ARG_D, ARG_NONE, "and d", OP_AND_A_REG},
{0xa3, 4, 0, 1,  false, ARG_E, ARG_NONE, "and e", OP_AND_A_REG},
{0xa4, 4, 0, 1,  false, ARG_H, ARG_NONE, "and h", OP_AND_A_REG},
{0xa5, 4, 0, 1,  false, ARG_L, ARG_NONE, "and l", OP_AND_A_REG},
{0xa6, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "and (hl)", OP_AND_A_IND_HL},
{0xa7, 4, 0, 1,  false, ARG_A, ARG_NONE, "and a", OP_AND_A_REG},
{0xa8, 4, 0, 1,  false, ARG_B, ARG_NONE, "xor b", OP_XOR_REG},
{0xa9, 4, 0, 1,  false, ARG_C, ARG_NONE, "xor c", OP_XOR_REG},
{0xaa, 4, 0, 1,  false, ARG_D, ARG_NONE, "xor d", OP_XOR_REG},
{0xab, 4, 0, 1,  false, ARG_E, ARG_NONE, "xor e", OP_XOR_REG},
{0xac, 4, 0, 1,  false, ARG_H, ARG_NONE, "xor h", OP_XOR_REG},
{0xad, 4, 0, 1,  false, ARG_L, ARG_NONE, "xor l", OP_XOR_REG},
{0xae, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "xor (hl)", OP_XOR_IND_HL},
{0xaf, 4, 0, 1,  false, ARG_A, ARG_NONE, "xor a", OP_XOR_REG},
{0xb0, 4, 0, 1,  false, ARG_B, ARG_NONE, "or b", OP_OR_REG},
{0xb1, 4, 0, 1,  false, ARG_C, ARG_NONE, "or c", OP_OR_REG},
{0xb2, 4, 0, 1,  false, ARG_D, ARG_NONE, "or d", OP_OR_REG},
{0xb3, 4, 0, 1,  false, ARG_E, ARG_NONE, "or e", OP_OR_REG},
{0xb4, 4, 0, 1,  false, ARG_H, ARG_NONE, "or h", OP_OR_REG},
{0xb5, 4, 0, 1,  false, ARG_L, ARG_NONE, "or l", OP_OR_REG},
{0xb6, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "or (hl)", OP_OR_IND_HL},
{0xb7, 4, 0, 1,  false, ARG_A, ARG_NONE, "or a", OP_OR_REG},
{0xb8, 4, 0, 1,  false, ARG_B, ARG_NONE, "cp b", OP_CP_A_REG},
{0xb9, 4, 0, 1,  false, ARG_C, ARG_NONE, "cp c", OP_CP_A_REG},
{0xba, 4, 0, 1,  false, ARG_D, ARG_NONE, "cp d", OP_CP_A_REG},
{0xbb, 4, 0, 1,  false, ARG_E, ARG_NONE, "cp e", OP_CP_A_REG},
{0xbc, 4, 0, 1,  false, ARG_H, ARG_NONE, "cp h", OP_CP_A_REG},
{0xbd, 4, 0, 1,  false, ARG_L, ARG_NONE, "cp l", OP_CP_A_REG},
{0xbe, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "cp (hl)", OP_CP_A_IND_HL},
{0xbf, 4, 0, 1,  false, ARG_A, ARG_NONE, "cp a", OP_CP_A_REG},
{0xc0, 20, 8, 1,  false, ARG_NZ, ARG_NONE, "ret nz", OP_RET_NZ},
{0xc1, 12, 0, 1,  false, ARG_BC, ARG_NONE, "pop bc", OP_POP},
{0xc2, 16, 12, 3,  false, ARG_NZ, ARG_A16, "jp nz, 0x%04x", OP_JP_NZ_16IM},
{0xc3, 16, 0, 3,  false, ARG_A16, ARG_NONE, "jp 0x%04x", OP_JP_16IM},
{0xc4, 24, 12, 3,  false, ARG_NZ, ARG_A16, "call nz, 0x%04x", OP_CALL_NZ_16IM},
{0xc5, 16, 0, 1,  false, ARG_BC, ARG_NONE, "push bc", OP_PUSH},
{0xc6, 8, 0, 2,  false, ARG_A, ARG_D8, "add a, 0x%02x", OP_ADD_A_8IM},
{0xc7, 16, 0, 1,  false, ARG_00H, ARG_NONE, "rst 00h", OP_RST},
{0xc8, 20, 8, 1,  false, ARG_Z, ARG_NONE, "ret z", OP_RET_Z},
{0xc9, 16, 0, 1,  false, ARG_NONE, ARG_NONE, "ret", OP_RET},
{0xca, 16, 12, 3,  false, ARG_Z, ARG_A16, "jp z, 0x%04x", OP_JP_Z_16IM},
{0xcb, 4, 0, 1,  false, ARG_CB, ARG_NONE, "prefix cb", OP_TMP},
{0xcc, 24, 12, 3,  false, ARG_Z, ARG_A16, "call z, 0x%04x", OP_CALL_Z_16IM},
{0xcd, 24, 0, 3,  false, ARG_A16, ARG_NONE, "call 0x%04x", OP_CALL_16IM},
{0xce, 8, 0, 2,  false, ARG_A, ARG_D8, "adc a, 0x%04x", OP_ADC_A_8IM},
{0xcf, 16, 0, 1,  false, ARG_08H, ARG_NONE, "rst 08h", OP_RST},
{0xd0, 20, 8, 1,  false, ARG_NC, ARG_NONE, "ret nc", OP_RET_NC},
{0xd1, 12, 0, 1,  false, ARG_DE, ARG_NONE, "pop de", OP_POP},
{0xd2, 16, 12, 3,  false, ARG_NC, ARG_A16, "jp nc, 0x%04x", OP_JP_NC_16IM},
{0xd3, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xd4, 24, 12, 3,  false, ARG_NC, ARG_A16, "call nc, 0x%04x", OP_CALL_NC_16IM},
{0xd5, 16, 0, 1,  false, ARG_DE, ARG_NONE, "push de", OP_PUSH},
{0xd6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "sub 0x%02x", OP_SUB_8IM},
{0xd7, 16, 0, 1,  false, ARG_10H, ARG_NONE, "rst 10h", OP_RST},
{0xd8, 20, 8, 1,  false, ARG_C, ARG_NONE, "ret c", OP_RET_C},
{0xd9, 16, 0, 1,  false, ARG_NONE, ARG_NONE, "reti", OP_RETI},
{0xda, 16, 12, 3,  false, ARG_C, ARG_A16, "jp c, 0x%04x", OP_JP_C_16IM},
{0xdb, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xdc, 24, 12, 3,  false, ARG_C, ARG_A16, "call c, 0x%04x", OP_CALL_C_16IM},
{0xdd, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xde, 8, 0, 2,  false, ARG_A, ARG_D8, "sbc a, 0x%02x", OP_SBC_8IM},
{0xdf, 16, 0, 1,  false, ARG_18H, ARG_NONE, "rst 18h", OP_RST},
{0xe0, 12, 0, 2,  false, ARG_IND_A8, ARG_A, "ldh (0x%04x), a", OP_LDH_8IM_A},
{0xe1, 12, 0, 1,  false, ARG_HL, ARG_NONE, "pop hl", OP_POP},
{0xe2, 8, 0, 1,  false, ARG_IND_C, ARG_A, "ld (c), a", OP_LD_IND_C_A},
{0xe3, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xe4, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xe5, 16, 0, 1,  false, ARG_HL, ARG_NONE, "push hl", OP_PUSH},
{0xe6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "and 0x%02x", OP_AND_A_8IM},
{0xe7, 16, 0, 1,  false, ARG_20H, ARG_NONE, "rst 20h", OP_RST},
{0xe8, 16, 0, 2,  false, ARG_SP, ARG_R8, "add sp, 0x%04x", OP_ADD_SP_8IM},
{0xe9, 4, 0, 1,  false, ARG_IND_HL, ARG_NONE, "jp (hl)", OP_JP_IND_HL},
{0xea, 16, 0, 3,  false, ARG_IND_A16, ARG_A, "ld (0x%04x), a", OP_LD_IND_16IM_A},
{0xeb, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xec, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xed, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xee, 8, 0, 2,  false, ARG_D8, ARG_NONE, "xor 0x%02x", OP_XOR_8IM},
{0xef, 16, 0, 1,  false, ARG_28H, ARG_NONE, "rst 28h", OP_RST},
{0xf0, 12, 0, 2,  false, ARG_A, ARG_IND_A8, "ldh a, (0x%04x)", OP_LDH_A_8IM},
{0xf1, 12, 0, 1,  false, ARG_AF, ARG_NONE, "pop af", OP_POP},
{0xf2, 8, 0, 1,  false, ARG_A, ARG_IND_C, "ld a, (c)", OP_LD_A_IND_C},
{0xf3, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "di", OP_DI},
{0xf4, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xf5, 16, 0, 1,  false, ARG_AF, ARG_NONE, "push af", OP_PUSH},
{0xf6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "or 0x%02x", OP_OR_8IM},
{0xf7, 16, 0, 1,  false, ARG_30H, ARG_NONE, "rst 30h", OP_RST},
{0xf8, 12, 0, 2,  false, ARG_HL, ARG_SPIR8, "ld hl 0x%04x", OP_LD_HL_8IM},
{0xf9, 8, 0, 1,  false, ARG_SP, ARG_HL, "ld sp, hl", OP_LD_SP_HL},
{0xfa, 16, 0, 3,  false, ARG_A, ARG_IND_A16, "ld a, (0x%04x)", OP_LD_A_IND_IM16},
{0xfb, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "ei", OP_EI},
{0xfc, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xfd, 0, 0, 0, false, ARG_NONE, ARG_NONE, "not an op!", OP_TMP},
{0xfe, 8, 0, 2,  false, ARG_D8, ARG_NONE, "cp 0x%02x", OP_CP_A_8IM},
{0xff, 16, 0, 1,  false, ARG_38H, ARG_NONE, "rst 38h", OP_RST},
};

static const opcode cb_op_array[256] = {
{0x0, 8, 0, 2,  false, ARG_B, ARG_NONE, "rlc", OP_TMP},
{0x1, 8, 0, 2,  false, ARG_C, ARG_NONE, "rlc", OP_TMP},
{0x2, 8, 0, 2,  false, ARG_D, ARG_NONE, "rlc", OP_TMP},
{0x3, 8, 0, 2,  false, ARG_E, ARG_NONE, "rlc", OP_TMP},
{0x4, 8, 0, 2,  false, ARG_H, ARG_NONE, "rlc", OP_TMP},
{0x5, 8, 0, 2,  false, ARG_L, ARG_NONE, "rlc", OP_TMP},
{0x6, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "rlc", OP_TMP},
{0x7, 8, 0, 2,  false, ARG_A, ARG_NONE, "rlc", OP_TMP},
{0x8, 8, 0, 2,  false, ARG_B, ARG_NONE, "rrc", OP_TMP},
{0x9, 8, 0, 2,  false, ARG_C, ARG_NONE, "rrc", OP_TMP},
{0xa, 8, 0, 2,  false, ARG_D, ARG_NONE, "rrc", OP_TMP},
{0xb, 8, 0, 2,  false, ARG_E, ARG_NONE, "rrc", OP_TMP},
{0xc, 8, 0, 2,  false, ARG_H, ARG_NONE, "rrc", OP_TMP},
{0xd, 8, 0, 2,  false, ARG_L, ARG_NONE, "rrc", OP_TMP},
{0xe, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "rrc", OP_TMP},
{0xf, 8, 0, 2,  false, ARG_A, ARG_NONE, "rrc", OP_TMP},
{0x10, 8, 0, 2,  false, ARG_B, ARG_NONE, "rl", OP_TMP},
{0x11, 8, 0, 2,  false, ARG_C, ARG_NONE, "rl", OP_TMP},
{0x12, 8, 0, 2,  false, ARG_D, ARG_NONE, "rl", OP_TMP},
{0x13, 8, 0, 2,  false, ARG_E, ARG_NONE, "rl", OP_TMP},
{0x14, 8, 0, 2,  false, ARG_H, ARG_NONE, "rl", OP_TMP},
{0x15, 8, 0, 2,  false, ARG_L, ARG_NONE, "rl", OP_TMP},
{0x16, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "rl", OP_TMP},
{0x17, 8, 0, 2,  false, ARG_A, ARG_NONE, "rl", OP_TMP},
{0x18, 8, 0, 2,  false, ARG_B, ARG_NONE, "rr b", OP_RR_REG},
{0x19, 8, 0, 2,  false, ARG_C, ARG_NONE, "rr c", OP_RR_REG},
{0x1a, 8, 0, 2,  false, ARG_D, ARG_NONE, "rr d", OP_RR_REG},
{0x1b, 8, 0, 2,  false, ARG_E, ARG_NONE, "rr e", OP_RR_REG},
{0x1c, 8, 0, 2,  false, ARG_H, ARG_NONE, "rr h", OP_RR_REG},
{0x1d, 8, 0, 2,  false, ARG_L, ARG_NONE, "rr l", OP_RR_REG},
{0x1e, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "rr (hl)", OP_RR_IND_HL},
{0x1f, 8, 0, 2,  false, ARG_A, ARG_NONE, "rr a", OP_TMP},
{0x20, 8, 0, 2,  false, ARG_B, ARG_NONE, "sla b", OP_SLA_REG},
{0x21, 8, 0, 2,  false, ARG_C, ARG_NONE, "sla c", OP_SLA_REG},
{0x22, 8, 0, 2,  false, ARG_D, ARG_NONE, "sla d", OP_SLA_REG},
{0x23, 8, 0, 2,  false, ARG_E, ARG_NONE, "sla e", OP_SLA_REG},
{0x24, 8, 0, 2,  false, ARG_H, ARG_NONE, "sla h", OP_SLA_REG},
{0x25, 8, 0, 2,  false, ARG_L, ARG_NONE, "sla l", OP_SLA_REG},
{0x26, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "sla", OP_TMP},
{0x27, 8, 0, 2,  false, ARG_A, ARG_NONE, "sla a", OP_SLA_REG},
{0x28, 8, 0, 2,  false, ARG_B, ARG_NONE, "sra", OP_TMP},
{0x29, 8, 0, 2,  false, ARG_C, ARG_NONE, "sra", OP_TMP},
{0x2a, 8, 0, 2,  false, ARG_D, ARG_NONE, "sra", OP_TMP},
{0x2b, 8, 0, 2,  false, ARG_E, ARG_NONE, "sra", OP_TMP},
{0x2c, 8, 0, 2,  false, ARG_H, ARG_NONE, "sra", OP_TMP},
{0x2d, 8, 0, 2,  false, ARG_L, ARG_NONE, "sra", OP_TMP},
{0x2e, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "sra", OP_TMP},
{0x2f, 8, 0, 2,  false, ARG_A, ARG_NONE, "sra", OP_TMP},
{0x30, 8, 0, 2,  false, ARG_B, ARG_NONE, "swap b", OP_SWAP_REG},
{0x31, 8, 0, 2,  false, ARG_C, ARG_NONE, "swap c", OP_SWAP_REG},
{0x32, 8, 0, 2,  false, ARG_D, ARG_NONE, "swap d", OP_SWAP_REG},
{0x33, 8, 0, 2,  false, ARG_E, ARG_NONE, "swap e", OP_SWAP_REG},
{0x34, 8, 0, 2,  false, ARG_H, ARG_NONE, "swap h", OP_SWAP_REG},
{0x35, 8, 0, 2,  false, ARG_L, ARG_NONE, "swap l", OP_SWAP_REG},
{0x36, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "swap (hl)", OP_SWAP_IND_HL},
{0x37, 8, 0, 2,  false, ARG_A, ARG_NONE, "swap a", OP_SWAP_REG},
{0x38, 8, 0, 2,  false, ARG_B, ARG_NONE, "srl b", OP_SRL_REG},
{0x39, 8, 0, 2,  false, ARG_C, ARG_NONE, "srl c", OP_SRL_REG},
{0x3a, 8, 0, 2,  false, ARG_D, ARG_NONE, "srl d", OP_SRL_REG},
{0x3b, 8, 0, 2,  false, ARG_E, ARG_NONE, "srl e", OP_SRL_REG},
{0x3c, 8, 0, 2,  false, ARG_H, ARG_NONE, "srl h", OP_SRL_REG},
{0x3d, 8, 0, 2,  false, ARG_L, ARG_NONE, "srl l", OP_SRL_REG},
{0x3e, 16, 0, 2,  false, ARG_IND_HL, ARG_NONE, "srl (hl)", OP_SRL_IND_HL},
{0x3f, 8, 0, 2,  false, ARG_A, ARG_NONE, "srl a", OP_SRL_REG},
{0x40, 8, 0, 2,  false, ARG_0, ARG_B, "bit 0, b", OP_BIT_NUM_REG},
{0x41, 8, 0, 2,  false, ARG_0, ARG_C, "bit 0, c", OP_BIT_NUM_REG},
{0x42, 8, 0, 2,  false, ARG_0, ARG_D, "bit 0, d", OP_BIT_NUM_REG},
{0x43, 8, 0, 2,  false, ARG_0, ARG_E, "bit 0, e", OP_BIT_NUM_REG},
{0x44, 8, 0, 2,  false, ARG_0, ARG_H, "bit 0, h", OP_BIT_NUM_REG},
{0x45, 8, 0, 2,  false, ARG_0, ARG_L, "bit 0, l", OP_BIT_NUM_REG},
{0x46, 16, 0, 2,  false, ARG_0, ARG_IND_HL, "bit 0, (hl)", OP_BIT_NUM_IND_HL},
{0x47, 8, 0, 2,  false, ARG_0, ARG_A, "bit 0, a", OP_BIT_NUM_REG},
{0x48, 8, 0, 2,  false, ARG_1, ARG_B, "bit 1, b", OP_BIT_NUM_REG},
{0x49, 8, 0, 2,  false, ARG_1, ARG_C, "bit 1, c", OP_BIT_NUM_REG},
{0x4a, 8, 0, 2,  false, ARG_1, ARG_D, "bit 1, d", OP_BIT_NUM_REG},
{0x4b, 8, 0, 2,  false, ARG_1, ARG_E, "bit 1, e", OP_BIT_NUM_REG},
{0x4c, 8, 0, 2,  false, ARG_1, ARG_H, "bit 1, h", OP_BIT_NUM_REG},
{0x4d, 8, 0, 2,  false, ARG_1, ARG_L, "bit 1, l", OP_BIT_NUM_REG},
{0x4e, 16, 0, 2,  false, ARG_1, ARG_IND_HL, "bit 1, (hl)", OP_BIT_NUM_IND_HL},
{0x4f, 8, 0, 2,  false, ARG_1, ARG_A, "bit 1, a", OP_BIT_NUM_REG},
{0x50, 8, 0, 2,  false, ARG_2, ARG_B, "bit 2, b", OP_BIT_NUM_REG},
{0x51, 8, 0, 2,  false, ARG_2, ARG_C, "bit 2, c", OP_BIT_NUM_REG},
{0x52, 8, 0, 2,  false, ARG_2, ARG_D, "bit 2, d", OP_BIT_NUM_REG},
{0x53, 8, 0, 2,  false, ARG_2, ARG_E, "bit 2, e", OP_BIT_NUM_REG},
{0x54, 8, 0, 2,  false, ARG_2, ARG_H, "bit 2, h", OP_BIT_NUM_REG},
{0x55, 8, 0, 2,  false, ARG_2, ARG_L, "bit 2, l", OP_BIT_NUM_REG},
{0x56, 16, 0, 2,  false, ARG_2, ARG_IND_HL, "bit 2, (hl)", OP_BIT_NUM_IND_HL},
{0x57, 8, 0, 2,  false, ARG_2, ARG_A, "bit 2, a", OP_BIT_NUM_REG},
{0x58, 8, 0, 2,  false, ARG_3, ARG_B, "bit 3, b", OP_BIT_NUM_REG},
{0x59, 8, 0, 2,  false, ARG_3, ARG_C, "bit 3, c", OP_BIT_NUM_REG},
{0x5a, 8, 0, 2,  false, ARG_3, ARG_D, "bit 3, d", OP_BIT_NUM_REG},
{0x5b, 8, 0, 2,  false, ARG_3, ARG_E, "bit 3, e", OP_BIT_NUM_REG},
{0x5c, 8, 0, 2,  false, ARG_3, ARG_H, "bit 3, h", OP_BIT_NUM_REG},
{0x5d, 8, 0, 2,  false, ARG_3, ARG_L, "bit 3, l", OP_BIT_NUM_REG},
{0x5e, 16, 0, 2,  false, ARG_3, ARG_IND_HL, "bit 3, (hl)", OP_BIT_NUM_IND_HL},
{0x5f, 8, 0, 2,  false, ARG_3, ARG_A, "bit 3, a", OP_BIT_NUM_REG},
{0x60, 8, 0, 2,  false, ARG_4, ARG_B, "bit 4, b", OP_BIT_NUM_REG},
{0x61, 8, 0, 2,  false, ARG_4, ARG_C, "bit 4, c", OP_BIT_NUM_REG},
{0x62, 8, 0, 2,  false, ARG_4, ARG_D, "bit 4, d", OP_BIT_NUM_REG},
{0x63, 8, 0, 2,  false, ARG_4, ARG_E, "bit 4, e", OP_BIT_NUM_REG},
{0x64, 8, 0, 2,  false, ARG_4, ARG_H, "bit 4, h", OP_BIT_NUM_REG},
{0x65, 8, 0, 2,  false, ARG_4, ARG_L, "bit 4, l", OP_BIT_NUM_REG},
{0x66, 16, 0, 2,  false, ARG_4, ARG_IND_HL, "bit 4, (hl)", OP_BIT_NUM_IND_HL},
{0x67, 8, 0, 2,  false, ARG_4, ARG_A, "bit 4, a", OP_BIT_NUM_REG},
{0x68, 8, 0, 2,  false, ARG_5, ARG_B, "bit 5, b", OP_BIT_NUM_REG},
{0x69, 8, 0, 2,  false, ARG_5, ARG_C, "bit 5, c", OP_BIT_NUM_REG},
{0x6a, 8, 0, 2,  false, ARG_5, ARG_D, "bit 5, d", OP_BIT_NUM_REG},
{0x6b, 8, 0, 2,  false, ARG_5, ARG_E, "bit 5, e", OP_BIT_NUM_REG},
{0x6c, 8, 0, 2,  false, ARG_5, ARG_H, "bit 5, h", OP_BIT_NUM_REG},
{0x6d, 8, 0, 2,  false, ARG_5, ARG_L, "bit 5, l", OP_BIT_NUM_REG},
{0x6e, 16, 0, 2,  false, ARG_5, ARG_IND_HL, "bit 5, (hl)", OP_BIT_NUM_IND_HL},
{0x6f, 8, 0, 2,  false, ARG_5, ARG_A, "bit 5, a", OP_BIT_NUM_REG},
{0x70, 8, 0, 2,  false, ARG_6, ARG_B, "bit 6, b", OP_BIT_NUM_REG},
{0x71, 8, 0, 2,  false, ARG_6, ARG_C, "bit 6, c", OP_BIT_NUM_REG},
{0x72, 8, 0, 2,  false, ARG_6, ARG_D, "bit 6, d", OP_BIT_NUM_REG},
{0x73, 8, 0, 2,  false, ARG_6, ARG_E, "bit 6, e", OP_BIT_NUM_REG},
{0x74, 8, 0, 2,  false, ARG_6, ARG_H, "bit 6, h", OP_BIT_NUM_REG},
{0x75, 8, 0, 2,  false, ARG_6, ARG_L, "bit 6, l", OP_BIT_NUM_REG},
{0x76, 16, 0, 2,  false, ARG_6, ARG_IND_HL, "bit 6, (hl)", OP_BIT_NUM_IND_HL},
{0x77, 8, 0, 2,  false, ARG_6, ARG_A, "bit 6, a", OP_BIT_NUM_REG},
{0x78, 8, 0, 2,  false, ARG_7, ARG_B, "bit 7, b", OP_BIT_NUM_REG},
{0x79, 8, 0, 2,  false, ARG_7, ARG_C, "bit 7, c", OP_BIT_NUM_REG},
{0x7a, 8, 0, 2,  false, ARG_7, ARG_D, "bit 7, d", OP_BIT_NUM_REG},
{0x7b, 8, 0, 2,  false, ARG_7, ARG_E, "bit 7, e", OP_BIT_NUM_REG},
{0x7c, 8, 0, 2,  false, ARG_7, ARG_H, "bit 7, h", OP_BIT_NUM_REG},
{0x7d, 8, 0, 2,  false, ARG_7, ARG_L, "bit 7, l", OP_BIT_NUM_REG},
{0x7e, 16, 0, 2,  false, ARG_7, ARG_IND_HL, "bit 7, (hl)", OP_BIT_NUM_IND_HL},
{0x7f, 8, 0, 2,  false, ARG_7, ARG_A, "bit 7, a", OP_BIT_NUM_REG},
{0x80, 8, 0, 2,  false, ARG_0, ARG_B, "res 0, b", OP_RES_BIT_REG},
{0x81, 8, 0, 2,  false, ARG_0, ARG_C, "res 0, c", OP_RES_BIT_REG},
{0x82, 8, 0, 2,  false, ARG_0, ARG_D, "res 0, d", OP_RES_BIT_REG},
{0x83, 8, 0, 2,  false, ARG_0, ARG_E, "res 0, e", OP_RES_BIT_REG},
{0x84, 8, 0, 2,  false, ARG_0, ARG_H, "res 0, h", OP_RES_BIT_REG},
{0x85, 8, 0, 2,  false, ARG_0, ARG_L, "res 0, l", OP_RES_BIT_REG},
{0x86, 16, 0, 2,  false, ARG_0, ARG_IND_HL, "res 0, (hl)", OP_RES_BIT_IND_HL},
{0x87, 8, 0, 2,  false, ARG_0, ARG_A, "res 0, a", OP_RES_BIT_REG},
{0x88, 8, 0, 2,  false, ARG_1, ARG_B, "res 1, b", OP_RES_BIT_REG},
{0x89, 8, 0, 2,  false, ARG_1, ARG_C, "res 1, c", OP_RES_BIT_REG},
{0x8a, 8, 0, 2,  false, ARG_1, ARG_D, "res 1, d", OP_RES_BIT_REG},
{0x8b, 8, 0, 2,  false, ARG_1, ARG_E, "res 1, e", OP_RES_BIT_REG},
{0x8c, 8, 0, 2,  false, ARG_1, ARG_H, "res 1, h", OP_RES_BIT_REG},
{0x8d, 8, 0, 2,  false, ARG_1, ARG_L, "res 1, l", OP_RES_BIT_REG},
{0x8e, 16, 0, 2,  false, ARG_1, ARG_IND_HL, "res 1, (hl)", OP_RES_BIT_IND_HL},
{0x8f, 8, 0, 2,  false, ARG_1, ARG_A, "res 1, a", OP_RES_BIT_REG},
{0x90, 8, 0, 2,  false, ARG_2, ARG_B, "res 2, b", OP_RES_BIT_REG},
{0x91, 8, 0, 2,  false, ARG_2, ARG_C, "res 2, c", OP_RES_BIT_REG},
{0x92, 8, 0, 2,  false, ARG_2, ARG_D, "res 2, d", OP_RES_BIT_REG},
{0x93, 8, 0, 2,  false, ARG_2, ARG_E, "res 2, e", OP_RES_BIT_REG},
{0x94, 8, 0, 2,  false, ARG_2, ARG_H, "res 2, h", OP_RES_BIT_REG},
{0x95, 8, 0, 2,  false, ARG_2, ARG_L, "res 2, l", OP_RES_BIT_REG},
{0x96, 16, 0, 2,  false, ARG_2, ARG_IND_HL, "res 2, (hl)", OP_RES_BIT_IND_HL},
{0x97, 8, 0, 2,  false, ARG_2, ARG_A, "res 2, a", OP_RES_BIT_REG},
{0x98, 8, 0, 2,  false, ARG_3, ARG_B, "res 3, b", OP_RES_BIT_REG},
{0x99, 8, 0, 2,  false, ARG_3, ARG_C, "res 3, c", OP_RES_BIT_REG},
{0x9a, 8, 0, 2,  false, ARG_3, ARG_D, "res 3, d", OP_RES_BIT_REG},
{0x9b, 8, 0, 2,  false, ARG_3, ARG_E, "res 3, e", OP_RES_BIT_REG},
{0x9c, 8, 0, 2,  false, ARG_3, ARG_H, "res 3, h", OP_RES_BIT_REG},
{0x9d, 8, 0, 2,  false, ARG_3, ARG_L, "res 3, l", OP_RES_BIT_REG},
{0x9e, 16, 0, 2,  false, ARG_3, ARG_IND_HL, "res 3, (hl)", OP_RES_BIT_IND_HL},
{0x9f, 8, 0, 2,  false, ARG_3, ARG_A, "res 3, a", OP_RES_BIT_REG},
{0xa0, 8, 0, 2,  false, ARG_4, ARG_B, "res 4, b", OP_RES_BIT_REG},
{0xa1, 8, 0, 2,  false, ARG_4, ARG_C, "res 4, c", OP_RES_BIT_REG},
{0xa2, 8, 0, 2,  false, ARG_4, ARG_D, "res 4, d", OP_RES_BIT_REG},
{0xa3, 8, 0, 2,  false, ARG_4, ARG_E, "res 4, e", OP_RES_BIT_REG},
{0xa4, 8, 0, 2,  false, ARG_4, ARG_H, "res 4, h", OP_RES_BIT_REG},
{0xa5, 8, 0, 2,  false, ARG_4, ARG_L, "res 4, l", OP_RES_BIT_REG},
{0xa6, 16, 0, 2,  false, ARG_4, ARG_IND_HL, "res 4, (hl)", OP_RES_BIT_IND_HL},
{0xa7, 8, 0, 2,  false, ARG_4, ARG_A, "res 4, a", OP_RES_BIT_REG},
{0xa8, 8, 0, 2,  false, ARG_5, ARG_B, "res 5, b", OP_RES_BIT_REG},
{0xa9, 8, 0, 2,  false, ARG_5, ARG_C, "res 5, c", OP_RES_BIT_REG},
{0xaa, 8, 0, 2,  false, ARG_5, ARG_D, "res 5, d", OP_RES_BIT_REG},
{0xab, 8, 0, 2,  false, ARG_5, ARG_E, "res 5, e", OP_RES_BIT_REG},
{0xac, 8, 0, 2,  false, ARG_5, ARG_H, "res 5, h", OP_RES_BIT_REG},
{0xad, 8, 0, 2,  false, ARG_5, ARG_L, "res 5, l", OP_RES_BIT_REG},
{0xae, 16, 0, 2,  false, ARG_5, ARG_IND_HL, "res 5, (hl)", OP_RES_BIT_IND_HL},
{0xaf, 8, 0, 2,  false, ARG_5, ARG_A, "res 5, a", OP_RES_BIT_REG},
{0xb0, 8, 0, 2,  false, ARG_6, ARG_B, "res 6, b", OP_RES_BIT_REG},
{0xb1, 8, 0, 2,  false, ARG_6, ARG_C, "res 6, c", OP_RES_BIT_REG},
{0xb2, 8, 0, 2,  false, ARG_6, ARG_D, "res 6, d", OP_RES_BIT_REG},
{0xb3, 8, 0, 2,  false, ARG_6, ARG_E, "res 6, e", OP_RES_BIT_REG},
{0xb4, 8, 0, 2,  false, ARG_6, ARG_H, "res 6, h", OP_RES_BIT_REG},
{0xb5, 8, 0, 2,  false, ARG_6, ARG_L, "res 6, l", OP_RES_BIT_REG},
{0xb6, 16, 0, 2,  false, ARG_6, ARG_IND_HL, "res 6, (hl)", OP_RES_BIT_IND_HL},
{0xb7, 8, 0, 2,  false, ARG_6, ARG_A, "res 6, a", OP_RES_BIT_REG},
{0xb8, 8, 0, 2,  false, ARG_7, ARG_B, "res 7, b", OP_RES_BIT_REG},
{0xb9, 8, 0, 2,  false, ARG_7, ARG_C, "res 7, c", OP_RES_BIT_REG},
{0xba, 8, 0, 2,  false, ARG_7, ARG_D, "res 7, d", OP_RES_BIT_REG},
{0xbb, 8, 0, 2,  false, ARG_7, ARG_E, "res 7, e", OP_RES_BIT_REG},
{0xbc, 8, 0, 2,  false, ARG_7, ARG_H, "res 7, h", OP_RES_BIT_REG},
{0xbd, 8, 0, 2,  false, ARG_7, ARG_L, "res 7, l", OP_RES_BIT_REG},
{0xbe, 16, 0, 2,  false, ARG_7, ARG_IND_HL, "res 7, (hl)", OP_RES_BIT_IND_HL},
{0xbf, 8, 0, 2,  false, ARG_7, ARG_A, "res 7, a", OP_RES_BIT_REG},
{0xc0, 8, 0, 2,  false, ARG_0, ARG_B, "set 0, b", OP_SET_BIT_REG},
{0xc1, 8, 0, 2,  false, ARG_0, ARG_C, "set 0, c", OP_SET_BIT_REG},
{0xc2, 8, 0, 2,  false, ARG_0, ARG_D, "set 0, d", OP_SET_BIT_REG},
{0xc3, 8, 0, 2,  false, ARG_0, ARG_E, "set 0, e", OP_SET_BIT_REG},
{0xc4, 8, 0, 2,  false, ARG_0, ARG_H, "set 0, h", OP_SET_BIT_REG},
{0xc5, 8, 0, 2,  false, ARG_0, ARG_L, "set 0, l", OP_SET_BIT_REG},
{0xc6, 16, 0, 2,  false, ARG_0, ARG_IND_HL, "set 0, (hl)", OP_SET_BIT_IND_HL},
{0xc7, 8, 0, 2,  false, ARG_0, ARG_A, "set 0, a", OP_SET_BIT_REG},
{0xc8, 8, 0, 2,  false, ARG_1, ARG_B, "set 1, b", OP_SET_BIT_REG},
{0xc9, 8, 0, 2,  false, ARG_1, ARG_C, "set 1, c", OP_SET_BIT_REG},
{0xca, 8, 0, 2,  false, ARG_1, ARG_D, "set 1, d", OP_SET_BIT_REG},
{0xcb, 8, 0, 2,  false, ARG_1, ARG_E, "set 1, e", OP_SET_BIT_REG},
{0xcc, 8, 0, 2,  false, ARG_1, ARG_H, "set 1, h", OP_SET_BIT_REG},
{0xcd, 8, 0, 2,  false, ARG_1, ARG_L, "set 1, l", OP_SET_BIT_REG},
{0xce, 16, 0, 2,  false, ARG_1, ARG_IND_HL, "set 1, (hl)", OP_SET_BIT_IND_HL},
{0xcf, 8, 0, 2,  false, ARG_1, ARG_A, "set 1, a", OP_SET_BIT_REG},
{0xd0, 8, 0, 2,  false, ARG_2, ARG_B, "set 2, b", OP_SET_BIT_REG},
{0xd1, 8, 0, 2,  false, ARG_2, ARG_C, "set 2, c", OP_SET_BIT_REG},
{0xd2, 8, 0, 2,  false, ARG_2, ARG_D, "set 2, d", OP_SET_BIT_REG},
{0xd3, 8, 0, 2,  false, ARG_2, ARG_E, "set 2, e", OP_SET_BIT_REG},
{0xd4, 8, 0, 2,  false, ARG_2, ARG_H, "set 2, h", OP_SET_BIT_REG},
{0xd5, 8, 0, 2,  false, ARG_2, ARG_L, "set 2, l", OP_SET_BIT_REG},
{0xd6, 16, 0, 2,  false, ARG_2, ARG_IND_HL, "set 2, (hl)", OP_SET_BIT_IND_HL},
{0xd7, 8, 0, 2,  false, ARG_2, ARG_A, "set 2, a", OP_SET_BIT_REG},
{0xd8, 8, 0, 2,  false, ARG_3, ARG_B, "set 3, b", OP_SET_BIT_REG},
{0xd9, 8, 0, 2,  false, ARG_3, ARG_C, "set 3, c", OP_SET_BIT_REG},
{0xda, 8, 0, 2,  false, ARG_3, ARG_D, "set 3, d", OP_SET_BIT_REG},
{0xdb, 8, 0, 2,  false, ARG_3, ARG_E, "set 3, e", OP_SET_BIT_REG},
{0xdc, 8, 0, 2,  false, ARG_3, ARG_H, "set 3, h", OP_SET_BIT_REG},
{0xdd, 8, 0, 2,  false, ARG_3, ARG_L, "set 3, l", OP_SET_BIT_REG},
{0xde, 16, 0, 2,  false, ARG_3, ARG_IND_HL, "set 3, (hl)", OP_SET_BIT_IND_HL},
{0xdf, 8, 0, 2,  false, ARG_3, ARG_A, "set 3, a", OP_SET_BIT_REG},
{0xe0, 8, 0, 2,  false, ARG_4, ARG_B, "set 4, b", OP_SET_BIT_REG},
{0xe1, 8, 0, 2,  false, ARG_4, ARG_C, "set 4, c", OP_SET_BIT_REG},
{0xe2, 8, 0, 2,  false, ARG_4, ARG_D, "set 4, d", OP_SET_BIT_REG},
{0xe3, 8, 0, 2,  false, ARG_4, ARG_E, "set 4, e", OP_SET_BIT_REG},
{0xe4, 8, 0, 2,  false, ARG_4, ARG_H, "set 4, h", OP_SET_BIT_REG},
{0xe5, 8, 0, 2,  false, ARG_4, ARG_L, "set 4, l", OP_SET_BIT_REG},
{0xe6, 16, 0, 2,  false, ARG_4, ARG_IND_HL, "set 4, (hl)", OP_SET_BIT_IND_HL},
{0xe7, 8, 0, 2,  false, ARG_4, ARG_A, "set 4, a", OP_SET_BIT_REG},
{0xe8, 8, 0, 2,  false, ARG_5, ARG_B, "set 5, b", OP_SET_BIT_REG},
{0xe9, 8, 0, 2,  false, ARG_5, ARG_C, "set 5, c", OP_SET_BIT_REG},
{0xea, 8, 0, 2,  false, ARG_5, ARG_D, "set 5, d", OP_SET_BIT_REG},
{0xeb, 8, 0, 2,  false, ARG_5, ARG_E, "set 5, e", OP_SET_BIT_REG},
{0xec, 8, 0, 2,  false, ARG_5, ARG_H, "set 5, h", OP_SET_BIT_REG},
{0xed, 8, 0, 2,  false, ARG_5, ARG_L, "set 5, l", OP_SET_BIT_REG},
{0xee, 16, 0, 2,  false, ARG_5, ARG_IND_HL, "set 5, (hl)", OP_SET_BIT_IND_HL},
{0xef, 8, 0, 2,  false, ARG_5, ARG_A, "set 5, a", OP_SET_BIT_REG},
{0xf0, 8, 0, 2,  false, ARG_6, ARG_B, "set 6, b", OP_SET_BIT_REG},
{0xf1, 8, 0, 2,  false, ARG_6, ARG_C, "set 6, c", OP_SET_BIT_REG},
{0xf2, 8, 0, 2,  false, ARG_6, ARG_D, "set 6, d", OP_SET_BIT_REG},
{0xf3, 8, 0, 2,  false, ARG_6, ARG_E, "set 6, e", OP_SET_BIT_REG},
{0xf4, 8, 0, 2,  false, ARG_6, ARG_H, "set 6, h", OP_SET_BIT_REG},
{0xf5, 8, 0, 2,  false, ARG_6, ARG_L, "set 6, l", OP_SET_BIT_REG},
{0xf6, 16, 0, 2,  false, ARG_6, ARG_IND_HL, "set 6, (hl)", OP_SET_BIT_IND_HL},
{0xf7, 8, 0, 2,  false, ARG_6, ARG_A, "set 6, a", OP_SET_BIT_REG},
{0xf8, 8, 0, 2,  false, ARG_7, ARG_B, "set 7, b", OP_SET_BIT_REG},
{0xf9, 8, 0, 2,  false, ARG_7, ARG_C, "set 7, c", OP_SET_BIT_REG},
{0xfa, 8, 0, 2,  false, ARG_7, ARG_D, "set 7, d", OP_SET_BIT_REG},
{0xfb, 8, 0, 2,  false, ARG_7, ARG_E, "set 7, e", OP_SET_BIT_REG},
{0xfc, 8, 0, 2,  false, ARG_7, ARG_H, "set 7, h", OP_SET_BIT_REG},
{0xfd, 8, 0, 2,  false, ARG_7, ARG_L, "set 7, l", OP_SET_BIT_REG},
{0xfe, 16, 0, 2,  false, ARG_7, ARG_IND_HL, "set 7, (hl)", OP_SET_BIT_IND_HL},
{0xff, 8, 0, 2,  false, ARG_7, ARG_A, "set 7, a", OP_SET_BIT_REG},
};

#endif /* OPCODES_H */

