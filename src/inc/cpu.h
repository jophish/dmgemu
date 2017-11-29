/* Besides cpu-related definitions, this file contains a
bit of functionality in the way of some macros for getting/setting
registers and flags of the cpu*/

#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct cpu_regs {
  uint8_t a, f, b, c, d, e, h, l;
  uint16_t sp, pc;
} cpu_regs;

typedef struct clock {
  uint32_t m_cycles, cpu_cycles;
} clock;

typedef struct cpu {
  cpu_regs regs;
  clock clk;
} cpu;

// Here's our main cpu instance
cpu z80;

// Initializes registers to default values for DMG
// AF = 01B0h, BC = 0013h, DE = 00D8h
// HL = 014Dh, SP = FFFEh, PC = 0100h
void init_cpu();

// Prints the registers in the processor instance.
// If width == 1, prints 16-bit regs instead of 8-bit
void print_regs(bool width);

// Prints individual flags
void print_flags();


// Macros for setting/getting 16-bit registers,
// since the internal representation is 8-bit regs
#define set_AF(val)({				\
      uint16_t _val = (val);			\
      z80.regs.a = (uint8_t)(_val >> 8);	\
      z80.regs.f = (uint8_t)_val;})		\

#define set_BC(val)({				\
      uint16_t _val = (val);			\
      z80.regs.b = (uint8_t)(_val >> 8);	\
      z80.regs.c = (uint8_t)_val;})		\
    
#define set_DE(val)({				\
      uint16_t _val = (val);			\
      z80.regs.d = (uint8_t)(_val >> 8);	\
      z80.regs.e = (uint8_t)_val;})		\
    
#define set_HL(val)({				\
      uint16_t _val = (val);			\
      z80.regs.h = (uint8_t)(_val >> 8);	\
      z80.regs.l = (uint8_t)_val;})		\

// We have a special setter for the F register since
// the bottom four bits must always read zero
#define set_F(val)({			      	\
  uint8_t _val = (val);				\
  z80.regs.f = _val & (_val << 4);})		\


// These ones are just for the sake of consistency
#define set_SP(val)({				\
      uint16_t _val = (val);			\
      z80.regs.sp = _val;})			\
  
#define set_PC(val)({				\
      uint16_t _val = (val);			\
      z80.regs.pc = val;})			\

#define get_AF()				\
  ((uint16_t)(z80.regs.a) << 8) | z80.regs.f	\

#define get_BC()				\
  ((uint16_t)(z80.regs.b) << 8) | z80.regs.c	\

#define get_DE()				\
  ((uint16_t)(z80.regs.d) << 8) | z80.regs.e	\

#define get_HL()				\
  ((uint16_t)(z80.regs.h) << 8) | z80.regs.l	\

#define get_SP() z80.regs.sp

#define get_PC() z80.regs.pc
// Macros for setting/getting flags in register F

#define set_flag_Z(val)({						\
      bool _val = (val);						\
      z80.regs.f = (z80.regs.f & (~(1 << 7))) | (_val << 7);})	\

#define set_flag_N(val)({						\
      bool _val = (val);						\
      z80.regs.f = (z80.regs.f & (~(1 << 6))) | (_val << 6);})	\

#define set_flag_H(val)({						\
      bool _val = (val);						\
      z80.regs.f = (z80.regs.f & (~(1 << 5))) | (_val << 5);})	\

#define set_flag_C(val)({						\
      bool _val = (val);						\
      z80.regs.f = (z80.regs.f & (~(1 << 4))) | (_val << 4);})	\

#define get_flag_Z()				\
  ((z80.regs.f & (1 << 7)) >> 7)

#define get_flag_N()				\
  ((z80.regs.f & (1 << 6)) >> 6)

#define get_flag_H()				\
  ((z80.regs.f & (1 << 5)) >> 5)

#define get_flag_C()				\
  ((z80.regs.f & (1 << 4)) >> 4)

#endif /* CPU_H */