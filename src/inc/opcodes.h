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
  REG_CODE_D = 0b101,
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

enum qq_pair_code {
  QQ_PAIR_CODE_BC = 0b00,
  QQ_PAIR_CODE_DE = 0b01,
  QQ_PAIR_CODE_HL = 0b10,
  QQ_PAIR_CODE_AF = 0b11,
};


enum arg_enum {
  ARG_NONE,
  ARG_BC,
  ARG_D16,
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
  ARG_D8,
};

enum op_type {
  OP_NOP,
  OP_LD_DD_16IM,
  OP_TMP = 4,
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


int dd_to_reg_pair(cpu *z80_p, int dd_code, reg_pair *reg_pair_p);

// Given a cpu and a rom, will execute the instruction in the rom
// located at the current PC in the cpu. Returns the opcode of the
// instruction executed. If the opcode was a 0xCB prefixed op, then the
// MSB of the return value will be 0xCB, otherwise, 0.
int dispatch_op(emu *gb_emu_p, opcode *op_p);

// Given an opcode, returns the number of bytes it takes up,
// including any immediate values. The op argument is 2 bytes
// in order to account for 2-byte long opcodes, using the CB
// prefix.
int op_length(uint16_t op);

// Given an emu and a pc, gets the opcode at the specified pc. If
// the opcode is an 0xCB prefixed op, then the MSB of the return value
// will be 0xCB, otherwise, it will be 0.
uint16_t get_op(emu *gb_emu_p, uint16_t pc);

// Given a memory address, interprets it as an instruction and
// fills the buf with the string mnemonic of the opcode including
// any immediate data that might follow the code. Returns ERR_OP_INVALID_OR_NOT_IMPLEMENTED
// if the memory at addr does not correspond to a known instruction. Returns ERR_BUF_LEN
// if the given buffer is too small for the given string
int addr_to_op_str(emu *gb_emu_p, uint16_t addr, char *buf, int buf_len);

// Given bytes a and b to add together, checks if a half-carry will occur
bool check_hc_add(uint8_t a, uint8_t b);

// Masks
#define BYTE_MASK 0xFF
#define NIBBLE_MASK 0xF

// Extended prefix
#define PREFIX_CB 0xCB


// Opcode definitions

// 8-Bit Immediate Loads
#define OP_B8_LD_IV_A 0x3E
#define OP_B8_LD_IV_B 0x06
#define OP_B8_LD_IV_C 0x0E
#define OP_B8_LD_IV_D 0x16
#define OP_B8_LD_IV_E 0x1E
#define OP_B8_LD_IV_H 0x26
#define OP_B8_LD_IV_L 0x2E
#define OP_B8_LD_IV_IND_HL 0x36


// LD A, n
#define OP_LD_A_A 0x7F
#define OP_LD_A_B 0x78
#define OP_LD_A_C 0x79
#define OP_LD_A_D 0x7A
#define OP_LD_A_E 0x7B
#define OP_LD_A_H 0x7C
#define OP_LD_A_L 0x7D
#define OP_LD_A_IND_BC 0x0A
#define OP_LD_A_IND_DE 0x1A
#define OP_LD_A_IND_HL 0x7E
#define OP_LD_A_IND_NN 0xFA

// LDH n, A
#define OP_LDH_N_A 0xE0

// LDH, A, n
#define OP_LDH_A_N 0xF0

// LDH (C), A
#define OP_LDH_C_A 0xE2

// 16-Bit Immediate Loads
#define OP_B16_LD_IV_BC 0x01
#define OP_B16_LD_IV_DE 0x11
#define OP_B16_LD_IV_HL 0x21
#define OP_B16_LD_IV_SP 0x31
#define OP_B16_LD_IV_NN_A 0xEA

// CP
#define OP_B8_CP_IV_A 0xFE

// NOP
//#define OP_NOP        0x00

// Jump to 16-bit Immediate address
#define OP_B16_JP_IV 0xC3

// Relative jumps
#define OP_B8_JR_NZ 0x20

// XOR
#define OP_XOR_A 0xAF
#define OP_XOR_B 0xA8

// OR
#define OP_OR_A 0xB7
#define OP_OR_B 0xB0
#define OP_OR_C 0xB1
#define OP_OR_D 0xB2
#define OP_OR_E 0xB3
#define OP_OR_H 0xB4
#define OP_OR_L 0xB5
#define OP_OR_IND_HL 0xB6
#define OP_B8_OR_IV 0xF6

// LDD
#define OP_LDD_HL_A 0x32

// LDI
#define OP_LDI_A_HL 0x2A

// DEC
#define OP_DEC_A 0x3D
#define OP_DEC_B 0x05
#define OP_DEC_C 0x0D
#define OP_DEC_D 0x15
#define OP_DEC_E 0x1D
#define OP_DEC_H 0x25
#define OP_DEC_L 0x2D
#define OP_DEC_IND_HL 0x35

#define OP_DEC_BC 0x0B
#define OP_DEC_DE 0x1B
#define OP_DEC_HL 0x2B
#define OP_DEC_SP 0x3B

// INC
#define OP_INC_A 0x3C
#define OP_INC_B 0x04
#define OP_INC_C 0x0C
#define OP_INC_D 0x14
#define OP_INC_E 0x1C
#define OP_INC_H 0x24
#define OP_INC_L 0x2C
#define OP_INC_IND_HL 0x34

// Control
#define OP_DI 0xF3
#define OP_B16_CALL_IV 0xCD
#define OP_RET 0xC9

static const opcode op_array[256] = {
{0x0, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "nop", OP_NOP},
{0x1, 12, 0, 3,  false, ARG_BC, ARG_D16, "ld", OP_LD_DD_16IM},
{0x2, 8, 0, 1,  false, ARG_IND_BC, ARG_A, "ld", OP_TMP},
{0x3, 8, 0, 1,  false, ARG_BC, ARG_NONE, "inc", OP_TMP},
{0x4, 4, 0, 1,  false, ARG_B, ARG_NONE, "inc", OP_TMP},
{0x5, 4, 0, 1,  false, ARG_B, ARG_NONE, "dec", OP_TMP},
{0x6, 8, 0, 2,  false, ARG_B, ARG_D8, "ld", OP_TMP},
{0x7, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rlca", OP_TMP},
{0x8, 20, 0, 3,  false, ARG_IND_A16, ARG_SP, "ld", OP_TMP},
{0x9, 8, 0, 1,  false, ARG_HL, ARG_BC, "add", OP_TMP},
{0xa, 8, 0, 1,  false, ARG_A, ARG_IND_BC, "ld", OP_TMP},
{0xb, 8, 0, 1,  false, ARG_BC, ARG_NONE, "dec", OP_TMP},
{0xc, 4, 0, 1,  false, ARG_C, ARG_NONE, "inc", OP_TMP},
{0xd, 4, 0, 1,  false, ARG_C, ARG_NONE, "dec", OP_TMP},
{0xe, 8, 0, 2,  false, ARG_C, ARG_D8, "ld", OP_TMP},
{0xf, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rrca", OP_TMP},
{0x10, 4, 0, 2,  false, ARG_0, ARG_NONE, "stop", OP_TMP},
{0x11, 12, 0, 3,  false, ARG_DE, ARG_D16, "ld", OP_LD_DD_16IM},
{0x12, 8, 0, 1,  false, ARG_IND_DE, ARG_A, "ld", OP_TMP},
{0x13, 8, 0, 1,  false, ARG_DE, ARG_NONE, "inc", OP_TMP},
{0x14, 4, 0, 1,  false, ARG_D, ARG_NONE, "inc", OP_TMP},
{0x15, 4, 0, 1,  false, ARG_D, ARG_NONE, "dec", OP_TMP},
{0x16, 8, 0, 2,  false, ARG_D, ARG_D8, "ld", OP_TMP},
{0x17, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rla", OP_TMP},
{0x18, 12, 0, 2,  false, ARG_R8, ARG_NONE, "jr", OP_TMP},
{0x19, 8, 0, 1,  false, ARG_HL, ARG_DE, "add", OP_TMP},
{0x1a, 8, 0, 1,  false, ARG_A, ARG_IND_DE, "ld", OP_TMP},
{0x1b, 8, 0, 1,  false, ARG_DE, ARG_NONE, "dec", OP_TMP},
{0x1c, 4, 0, 1,  false, ARG_E, ARG_NONE, "inc", OP_TMP},
{0x1d, 4, 0, 1,  false, ARG_E, ARG_NONE, "dec", OP_TMP},
{0x1e, 8, 0, 2,  false, ARG_E, ARG_D8, "ld", OP_TMP},
{0x1f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "rra", OP_TMP},
{0x20, 12, 8, 2,  false, ARG_NZ, ARG_R8, "jr", OP_TMP},
{0x21, 12, 0, 3,  false, ARG_HL, ARG_D16, "ld", OP_LD_DD_16IM},
{0x22, 8, 0, 1,  false, ARG_IND_HLI, ARG_A, "ld", OP_TMP},
{0x23, 8, 0, 1,  false, ARG_HL, ARG_NONE, "inc", OP_TMP},
{0x24, 4, 0, 1,  false, ARG_H, ARG_NONE, "inc", OP_TMP},
{0x25, 4, 0, 1,  false, ARG_H, ARG_NONE, "dec", OP_TMP},
{0x26, 8, 0, 2,  false, ARG_H, ARG_D8, "ld", OP_TMP},
{0x27, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "daa", OP_TMP},
{0x28, 12, 8, 2,  false, ARG_Z, ARG_R8, "jr", OP_TMP},
{0x29, 8, 0, 1,  false, ARG_HL, ARG_HL, "add", OP_TMP},
{0x2a, 8, 0, 1,  false, ARG_A, ARG_IND_HLI, "ld", OP_TMP},
{0x2b, 8, 0, 1,  false, ARG_HL, ARG_NONE, "dec", OP_TMP},
{0x2c, 4, 0, 1,  false, ARG_L, ARG_NONE, "inc", OP_TMP},
{0x2d, 4, 0, 1,  false, ARG_L, ARG_NONE, "dec", OP_TMP},
{0x2e, 8, 0, 2,  false, ARG_L, ARG_D8, "ld", OP_TMP},
{0x2f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "cpl", OP_TMP},
{0x30, 12, 8, 2,  false, ARG_NC, ARG_R8, "jr", OP_TMP},
{0x31, 12, 0, 3,  false, ARG_SP, ARG_D16, "ld", OP_LD_DD_16IM},
{0x32, 8, 0, 1,  false, ARG_IND_HLD, ARG_A, "ld", OP_TMP},
{0x33, 8, 0, 1,  false, ARG_SP, ARG_NONE, "inc", OP_TMP},
{0x34, 12, 0, 1,  false, ARG_IND_HL, ARG_NONE, "inc", OP_TMP},
{0x35, 12, 0, 1,  false, ARG_IND_HL, ARG_NONE, "dec", OP_TMP},
{0x36, 12, 0, 2,  false, ARG_IND_HL, ARG_D8, "ld", OP_TMP},
{0x37, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "scf", OP_TMP},
{0x38, 12, 8, 2,  false, ARG_C, ARG_R8, "jr", OP_TMP},
{0x39, 8, 0, 1,  false, ARG_HL, ARG_SP, "add", OP_TMP},
{0x3a, 8, 0, 1,  false, ARG_A, ARG_IND_HLD, "ld", OP_TMP},
{0x3b, 8, 0, 1,  false, ARG_SP, ARG_NONE, "dec", OP_TMP},
{0x3c, 4, 0, 1,  false, ARG_A, ARG_NONE, "inc", OP_TMP},
{0x3d, 4, 0, 1,  false, ARG_A, ARG_NONE, "dec", OP_TMP},
{0x3e, 8, 0, 2,  false, ARG_A, ARG_D8, "ld", OP_TMP},
{0x3f, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "ccf", OP_TMP},
{0x40, 4, 0, 1,  false, ARG_B, ARG_B, "ld", OP_TMP},
{0x41, 4, 0, 1,  false, ARG_B, ARG_C, "ld", OP_TMP},
{0x42, 4, 0, 1,  false, ARG_B, ARG_D, "ld", OP_TMP},
{0x43, 4, 0, 1,  false, ARG_B, ARG_E, "ld", OP_TMP},
{0x44, 4, 0, 1,  false, ARG_B, ARG_H, "ld", OP_TMP},
{0x45, 4, 0, 1,  false, ARG_B, ARG_L, "ld", OP_TMP},
{0x46, 8, 0, 1,  false, ARG_B, ARG_IND_HL, "ld", OP_TMP},
{0x47, 4, 0, 1,  false, ARG_B, ARG_A, "ld", OP_TMP},
{0x48, 4, 0, 1,  false, ARG_C, ARG_B, "ld", OP_TMP},
{0x49, 4, 0, 1,  false, ARG_C, ARG_C, "ld", OP_TMP},
{0x4a, 4, 0, 1,  false, ARG_C, ARG_D, "ld", OP_TMP},
{0x4b, 4, 0, 1,  false, ARG_C, ARG_E, "ld", OP_TMP},
{0x4c, 4, 0, 1,  false, ARG_C, ARG_H, "ld", OP_TMP},
{0x4d, 4, 0, 1,  false, ARG_C, ARG_L, "ld", OP_TMP},
{0x4e, 8, 0, 1,  false, ARG_C, ARG_IND_HL, "ld", OP_TMP},
{0x4f, 4, 0, 1,  false, ARG_C, ARG_A, "ld", OP_TMP},
{0x50, 4, 0, 1,  false, ARG_D, ARG_B, "ld", OP_TMP},
{0x51, 4, 0, 1,  false, ARG_D, ARG_C, "ld", OP_TMP},
{0x52, 4, 0, 1,  false, ARG_D, ARG_D, "ld", OP_TMP},
{0x53, 4, 0, 1,  false, ARG_D, ARG_E, "ld", OP_TMP},
{0x54, 4, 0, 1,  false, ARG_D, ARG_H, "ld", OP_TMP},
{0x55, 4, 0, 1,  false, ARG_D, ARG_L, "ld", OP_TMP},
{0x56, 8, 0, 1,  false, ARG_D, ARG_IND_HL, "ld", OP_TMP},
{0x57, 4, 0, 1,  false, ARG_D, ARG_A, "ld", OP_TMP},
{0x58, 4, 0, 1,  false, ARG_E, ARG_B, "ld", OP_TMP},
{0x59, 4, 0, 1,  false, ARG_E, ARG_C, "ld", OP_TMP},
{0x5a, 4, 0, 1,  false, ARG_E, ARG_D, "ld", OP_TMP},
{0x5b, 4, 0, 1,  false, ARG_E, ARG_E, "ld", OP_TMP},
{0x5c, 4, 0, 1,  false, ARG_E, ARG_H, "ld", OP_TMP},
{0x5d, 4, 0, 1,  false, ARG_E, ARG_L, "ld", OP_TMP},
{0x5e, 8, 0, 1,  false, ARG_E, ARG_IND_HL, "ld", OP_TMP},
{0x5f, 4, 0, 1,  false, ARG_E, ARG_A, "ld", OP_TMP},
{0x60, 4, 0, 1,  false, ARG_H, ARG_B, "ld", OP_TMP},
{0x61, 4, 0, 1,  false, ARG_H, ARG_C, "ld", OP_TMP},
{0x62, 4, 0, 1,  false, ARG_H, ARG_D, "ld", OP_TMP},
{0x63, 4, 0, 1,  false, ARG_H, ARG_E, "ld", OP_TMP},
{0x64, 4, 0, 1,  false, ARG_H, ARG_H, "ld", OP_TMP},
{0x65, 4, 0, 1,  false, ARG_H, ARG_L, "ld", OP_TMP},
{0x66, 8, 0, 1,  false, ARG_H, ARG_IND_HL, "ld", OP_TMP},
{0x67, 4, 0, 1,  false, ARG_H, ARG_A, "ld", OP_TMP},
{0x68, 4, 0, 1,  false, ARG_L, ARG_B, "ld", OP_TMP},
{0x69, 4, 0, 1,  false, ARG_L, ARG_C, "ld", OP_TMP},
{0x6a, 4, 0, 1,  false, ARG_L, ARG_D, "ld", OP_TMP},
{0x6b, 4, 0, 1,  false, ARG_L, ARG_E, "ld", OP_TMP},
{0x6c, 4, 0, 1,  false, ARG_L, ARG_H, "ld", OP_TMP},
{0x6d, 4, 0, 1,  false, ARG_L, ARG_L, "ld", OP_TMP},
{0x6e, 8, 0, 1,  false, ARG_L, ARG_IND_HL, "ld", OP_TMP},
{0x6f, 4, 0, 1,  false, ARG_L, ARG_A, "ld", OP_TMP},
{0x70, 8, 0, 1,  false, ARG_IND_HL, ARG_B, "ld", OP_TMP},
{0x71, 8, 0, 1,  false, ARG_IND_HL, ARG_C, "ld", OP_TMP},
{0x72, 8, 0, 1,  false, ARG_IND_HL, ARG_D, "ld", OP_TMP},
{0x73, 8, 0, 1,  false, ARG_IND_HL, ARG_E, "ld", OP_TMP},
{0x74, 8, 0, 1,  false, ARG_IND_HL, ARG_H, "ld", OP_TMP},
{0x75, 8, 0, 1,  false, ARG_IND_HL, ARG_L, "ld", OP_TMP},
{0x76, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "halt", OP_TMP},
{0x77, 8, 0, 1,  false, ARG_IND_HL, ARG_A, "ld", OP_TMP},
{0x78, 4, 0, 1,  false, ARG_A, ARG_B, "ld", OP_TMP},
{0x79, 4, 0, 1,  false, ARG_A, ARG_C, "ld", OP_TMP},
{0x7a, 4, 0, 1,  false, ARG_A, ARG_D, "ld", OP_TMP},
{0x7b, 4, 0, 1,  false, ARG_A, ARG_E, "ld", OP_TMP},
{0x7c, 4, 0, 1,  false, ARG_A, ARG_H, "ld", OP_TMP},
{0x7d, 4, 0, 1,  false, ARG_A, ARG_L, "ld", OP_TMP},
{0x7e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "ld", OP_TMP},
{0x7f, 4, 0, 1,  false, ARG_A, ARG_A, "ld", OP_TMP},
{0x80, 4, 0, 1,  false, ARG_A, ARG_B, "add", OP_TMP},
{0x81, 4, 0, 1,  false, ARG_A, ARG_C, "add", OP_TMP},
{0x82, 4, 0, 1,  false, ARG_A, ARG_D, "add", OP_TMP},
{0x83, 4, 0, 1,  false, ARG_A, ARG_E, "add", OP_TMP},
{0x84, 4, 0, 1,  false, ARG_A, ARG_H, "add", OP_TMP},
{0x85, 4, 0, 1,  false, ARG_A, ARG_L, "add", OP_TMP},
{0x86, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "add", OP_TMP},
{0x87, 4, 0, 1,  false, ARG_A, ARG_A, "add", OP_TMP},
{0x88, 4, 0, 1,  false, ARG_A, ARG_B, "adc", OP_TMP},
{0x89, 4, 0, 1,  false, ARG_A, ARG_C, "adc", OP_TMP},
{0x8a, 4, 0, 1,  false, ARG_A, ARG_D, "adc", OP_TMP},
{0x8b, 4, 0, 1,  false, ARG_A, ARG_E, "adc", OP_TMP},
{0x8c, 4, 0, 1,  false, ARG_A, ARG_H, "adc", OP_TMP},
{0x8d, 4, 0, 1,  false, ARG_A, ARG_L, "adc", OP_TMP},
{0x8e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "adc", OP_TMP},
{0x8f, 4, 0, 1,  false, ARG_A, ARG_A, "adc", OP_TMP},
{0x90, 4, 0, 1,  false, ARG_B, ARG_NONE, "sub", OP_TMP},
{0x91, 4, 0, 1,  false, ARG_C, ARG_NONE, "sub", OP_TMP},
{0x92, 4, 0, 1,  false, ARG_D, ARG_NONE, "sub", OP_TMP},
{0x93, 4, 0, 1,  false, ARG_E, ARG_NONE, "sub", OP_TMP},
{0x94, 4, 0, 1,  false, ARG_H, ARG_NONE, "sub", OP_TMP},
{0x95, 4, 0, 1,  false, ARG_L, ARG_NONE, "sub", OP_TMP},
{0x96, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "sub", OP_TMP},
{0x97, 4, 0, 1,  false, ARG_A, ARG_NONE, "sub", OP_TMP},
{0x98, 4, 0, 1,  false, ARG_A, ARG_B, "sbc", OP_TMP},
{0x99, 4, 0, 1,  false, ARG_A, ARG_C, "sbc", OP_TMP},
{0x9a, 4, 0, 1,  false, ARG_A, ARG_D, "sbc", OP_TMP},
{0x9b, 4, 0, 1,  false, ARG_A, ARG_E, "sbc", OP_TMP},
{0x9c, 4, 0, 1,  false, ARG_A, ARG_H, "sbc", OP_TMP},
{0x9d, 4, 0, 1,  false, ARG_A, ARG_L, "sbc", OP_TMP},
{0x9e, 8, 0, 1,  false, ARG_A, ARG_IND_HL, "sbc", OP_TMP},
{0x9f, 4, 0, 1,  false, ARG_A, ARG_A, "sbc", OP_TMP},
{0xa0, 4, 0, 1,  false, ARG_B, ARG_NONE, "and", OP_TMP},
{0xa1, 4, 0, 1,  false, ARG_C, ARG_NONE, "and", OP_TMP},
{0xa2, 4, 0, 1,  false, ARG_D, ARG_NONE, "and", OP_TMP},
{0xa3, 4, 0, 1,  false, ARG_E, ARG_NONE, "and", OP_TMP},
{0xa4, 4, 0, 1,  false, ARG_H, ARG_NONE, "and", OP_TMP},
{0xa5, 4, 0, 1,  false, ARG_L, ARG_NONE, "and", OP_TMP},
{0xa6, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "and", OP_TMP},
{0xa7, 4, 0, 1,  false, ARG_A, ARG_NONE, "and", OP_TMP},
{0xa8, 4, 0, 1,  false, ARG_B, ARG_NONE, "xor", OP_TMP},
{0xa9, 4, 0, 1,  false, ARG_C, ARG_NONE, "xor", OP_TMP},
{0xaa, 4, 0, 1,  false, ARG_D, ARG_NONE, "xor", OP_TMP},
{0xab, 4, 0, 1,  false, ARG_E, ARG_NONE, "xor", OP_TMP},
{0xac, 4, 0, 1,  false, ARG_H, ARG_NONE, "xor", OP_TMP},
{0xad, 4, 0, 1,  false, ARG_L, ARG_NONE, "xor", OP_TMP},
{0xae, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "xor", OP_TMP},
{0xaf, 4, 0, 1,  false, ARG_A, ARG_NONE, "xor", OP_TMP},
{0xb0, 4, 0, 1,  false, ARG_B, ARG_NONE, "or", OP_TMP},
{0xb1, 4, 0, 1,  false, ARG_C, ARG_NONE, "or", OP_TMP},
{0xb2, 4, 0, 1,  false, ARG_D, ARG_NONE, "or", OP_TMP},
{0xb3, 4, 0, 1,  false, ARG_E, ARG_NONE, "or", OP_TMP},
{0xb4, 4, 0, 1,  false, ARG_H, ARG_NONE, "or", OP_TMP},
{0xb5, 4, 0, 1,  false, ARG_L, ARG_NONE, "or", OP_TMP},
{0xb6, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "or", OP_TMP},
{0xb7, 4, 0, 1,  false, ARG_A, ARG_NONE, "or", OP_TMP},
{0xb8, 4, 0, 1,  false, ARG_B, ARG_NONE, "cp", OP_TMP},
{0xb9, 4, 0, 1,  false, ARG_C, ARG_NONE, "cp", OP_TMP},
{0xba, 4, 0, 1,  false, ARG_D, ARG_NONE, "cp", OP_TMP},
{0xbb, 4, 0, 1,  false, ARG_E, ARG_NONE, "cp", OP_TMP},
{0xbc, 4, 0, 1,  false, ARG_H, ARG_NONE, "cp", OP_TMP},
{0xbd, 4, 0, 1,  false, ARG_L, ARG_NONE, "cp", OP_TMP},
{0xbe, 8, 0, 1,  false, ARG_IND_HL, ARG_NONE, "cp", OP_TMP},
{0xbf, 4, 0, 1,  false, ARG_A, ARG_NONE, "cp", OP_TMP},
{0xc0, 20, 8, 1,  false, ARG_NZ, ARG_NONE, "ret", OP_TMP},
{0xc1, 12, 0, 1,  false, ARG_BC, ARG_NONE, "pop", OP_TMP},
{0xc2, 16, 12, 3,  false, ARG_NZ, ARG_A16, "jp", OP_TMP},
{0xc3, 16, 0, 3,  false, ARG_A16, ARG_NONE, "jp", OP_TMP},
{0xc4, 24, 12, 3,  false, ARG_NZ, ARG_A16, "call", OP_TMP},
{0xc5, 16, 0, 1,  false, ARG_BC, ARG_NONE, "push", OP_TMP},
{0xc6, 8, 0, 2,  false, ARG_A, ARG_D8, "add", OP_TMP},
{0xc7, 16, 0, 1,  false, ARG_00H, ARG_NONE, "rst", OP_TMP},
{0xc8, 20, 8, 1,  false, ARG_Z, ARG_NONE, "ret", OP_TMP},
{0xc9, 16, 0, 1,  false, ARG_NONE, ARG_NONE, "ret", OP_TMP},
{0xca, 16, 12, 3,  false, ARG_Z, ARG_A16, "jp", OP_TMP},
{0xcb, 4, 0, 1,  false, ARG_CB, ARG_NONE, "prefix", OP_TMP},
{0xcc, 24, 12, 3,  false, ARG_Z, ARG_A16, "call", OP_TMP},
{0xcd, 24, 0, 3,  false, ARG_A16, ARG_NONE, "call", OP_TMP},
{0xce, 8, 0, 2,  false, ARG_A, ARG_D8, "adc", OP_TMP},
{0xcf, 16, 0, 1,  false, ARG_08H, ARG_NONE, "rst", OP_TMP},
{0xd0, 20, 8, 1,  false, ARG_NC, ARG_NONE, "ret", OP_TMP},
{0xd1, 12, 0, 1,  false, ARG_DE, ARG_NONE, "pop", OP_TMP},
{0xd2, 16, 12, 3,  false, ARG_NC, ARG_A16, "jp", OP_TMP},
{0xd4, 24, 12, 3,  false, ARG_NC, ARG_A16, "call", OP_TMP},
{0xd5, 16, 0, 1,  false, ARG_DE, ARG_NONE, "push", OP_TMP},
{0xd6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "sub", OP_TMP},
{0xd7, 16, 0, 1,  false, ARG_10H, ARG_NONE, "rst", OP_TMP},
{0xd8, 20, 8, 1,  false, ARG_C, ARG_NONE, "ret", OP_TMP},
{0xd9, 16, 0, 1,  false, ARG_NONE, ARG_NONE, "reti", OP_TMP},
{0xda, 16, 12, 3,  false, ARG_C, ARG_A16, "jp", OP_TMP},
{0xdc, 24, 12, 3,  false, ARG_C, ARG_A16, "call", OP_TMP},
{0xde, 8, 0, 2,  false, ARG_A, ARG_D8, "sbc", OP_TMP},
{0xdf, 16, 0, 1,  false, ARG_18H, ARG_NONE, "rst", OP_TMP},
{0xe0, 12, 0, 2,  false, ARG_IND_A8, ARG_A, "ldh", OP_TMP},
{0xe1, 12, 0, 1,  false, ARG_HL, ARG_NONE, "pop", OP_TMP},
{0xe2, 8, 0, 2,  false, ARG_IND_C, ARG_A, "ld", OP_TMP},
{0xe5, 16, 0, 1,  false, ARG_HL, ARG_NONE, "push", OP_TMP},
{0xe6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "and", OP_TMP},
{0xe7, 16, 0, 1,  false, ARG_20H, ARG_NONE, "rst", OP_TMP},
{0xe8, 16, 0, 2,  false, ARG_SP, ARG_R8, "add", OP_TMP},
{0xe9, 4, 0, 1,  false, ARG_IND_HL, ARG_NONE, "jp", OP_TMP},
{0xea, 16, 0, 3,  false, ARG_IND_A16, ARG_A, "ld", OP_TMP},
{0xee, 8, 0, 2,  false, ARG_D8, ARG_NONE, "xor", OP_TMP},
{0xef, 16, 0, 1,  false, ARG_28H, ARG_NONE, "rst", OP_TMP},
{0xf0, 12, 0, 2,  false, ARG_A, ARG_IND_A8, "ldh", OP_TMP},
{0xf1, 12, 0, 1,  false, ARG_AF, ARG_NONE, "pop", OP_TMP},
{0xf2, 8, 0, 2,  false, ARG_A, ARG_IND_C, "ld", OP_TMP},
{0xf3, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "di", OP_TMP},
{0xf5, 16, 0, 1,  false, ARG_AF, ARG_NONE, "push", OP_TMP},
{0xf6, 8, 0, 2,  false, ARG_D8, ARG_NONE, "or", OP_TMP},
{0xf7, 16, 0, 1,  false, ARG_30H, ARG_NONE, "rst", OP_TMP},
{0xf8, 12, 0, 2,  false, ARG_HL, ARG_SPIR8, "ld", OP_TMP},
{0xf9, 8, 0, 1,  false, ARG_SP, ARG_HL, "ld", OP_TMP},
{0xfa, 16, 0, 3,  false, ARG_A, ARG_IND_A16, "ld", OP_TMP},
{0xfb, 4, 0, 1,  false, ARG_NONE, ARG_NONE, "ei", OP_TMP},
{0xfe, 8, 0, 2,  false, ARG_D8, ARG_NONE, "cp", OP_TMP},
{0xff, 16, 0, 1,  false, ARG_38H, ARG_NONE, "rst", OP_TMP},
};

static const opcode cb_op_array[256] = {};

#endif /* OPCODES_H */

