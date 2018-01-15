#ifndef OPCODES_H
#define OPCODES_H

#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "mmu.h"
#include "rom.h"
#include "emu.h"
#include "util.h"

// Given a cpu and a rom, will execute the instruction in the rom
// located at the current PC in the cpu. Returns the opcode of the
// instruction executed.
uint16_t dispatch_op(emu *gb_emu_p);

// Opcode definitions

// 8-Bit Immediate Loads
#define B8_LD_IV_B 0x06
#define B8_LD_IV_C 0x0E
#define B8_LD_IV_D 0x16
#define B8_LD_IV_E 0x1E
#define B8_LD_IV_H 0x26
#define B8_LD_IV_L 0x2E

// 16-Bit Immediate Loads
#define B16_LD_IV_BC 0x01
#define B16_LD_IV_DE 0x11
#define B16_LD_IV_HL 0x21
#define B16_LD_IV_SP 0x31

// NOP
#define NOP        0x00

// Jump to 16-bit Immediate address
#define B16_JP_IV 0xC3

// Relative jumps
#define B8_JR_NZ 0x20

// XOR
#define XOR_A 0xAF
#define XOR_B 0xA8

// LDD
#define LDD_HL_A 0x32


// DEC
#define DEC_A 0x3D
#define DEC_B 0x05
#define DEC_C 0x0D
#define DEC_D 0x15
#define DEC_E 0x1D
#define DEC_H 0x25
#define DEC_L 0x2D
#define DEC_IND_HL 0x35
#endif /* OPCODES_H */
