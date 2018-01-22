#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include <stdio.h>
#include "mmu.h"
#include "emu.h"
#include "cpu.h"
#include "rom.h"
#include "util.h"

// Given a cpu and a rom, will execute the instruction in the rom
// located at the current PC in the cpu. Returns the opcode of the
// instruction executed.
int dispatch_op(emu *gb_emu_p);

// Given an opcode, returns the number of bytes it takes up,
// including any immediate values. The op argument is 2 bytes
// in order to account for 2-byte long opcodes, using the CB
// prefix.
int op_length(uint16_t op);

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
#define OP_NOP        0x00

// Jump to 16-bit Immediate address
#define OP_B16_JP_IV 0xC3

// Relative jumps
#define OP_B8_JR_NZ 0x20

// XOR
#define OP_XOR_A 0xAF
#define OP_XOR_B 0xA8

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

#endif /* OPCODES_H */
