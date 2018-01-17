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

// Opcode definitions

// 8-Bit Immediate Loads
#define OP_B8_LD_IV_A 0x3E
#define OP_B8_LD_IV_B 0x06
#define OP_B8_LD_IV_C 0x0E
#define OP_B8_LD_IV_D 0x16
#define OP_B8_LD_IV_E 0x1E
#define OP_B8_LD_IV_H 0x26
#define OP_B8_LD_IV_L 0x2E

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

// 16-Bit Immediate Loads
#define OP_B16_LD_IV_BC 0x01
#define OP_B16_LD_IV_DE 0x11
#define OP_B16_LD_IV_HL 0x21
#define OP_B16_LD_IV_SP 0x31

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


// DEC
#define OP_DEC_A 0x3D
#define OP_DEC_B 0x05
#define OP_DEC_C 0x0D
#define OP_DEC_D 0x15
#define OP_DEC_E 0x1D
#define OP_DEC_H 0x25
#define OP_DEC_L 0x2D
#define OP_DEC_IND_HL 0x35

// Control
#define OP_DI 0xF3
#endif /* OPCODES_H */
