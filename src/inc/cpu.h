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
  bool ie;
} cpu_regs;

typedef struct clock {
  uint32_t m_cycles, cpu_cycles;
} clock;

typedef struct cpu {
  cpu_regs regs;
  clock clk;
} cpu;

// Initializes registers to default values for DMG
// AF = 01B0h, BC = 0013h, DE = 00D8h
// HL = 014Dh, SP = FFFEh, PC = 0100h
void init_cpu(cpu *z80);

// Prints the registers in the processor instance.
// If width == 1, prints 16-bit regs instead of 8-bit
void print_regs(cpu *z80, bool width);

// Prints individual flags
void print_flags(cpu *z80);


// Macros for setting/getting 16-bit registers,
// since the internal representation is 8-bit regs
#define set_AF(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.a = (uint8_t)(_val >> 8);	\
      z80->regs.f = (uint8_t)_val;})		\

#define set_BC(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.b = (uint8_t)(_val >> 8);	\
      z80->regs.c = (uint8_t)_val;})		\

#define set_DE(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.d = (uint8_t)(_val >> 8);	\
      z80->regs.e = (uint8_t)_val;})		\

#define set_HL(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.h = (uint8_t)(_val >> 8);	\
      z80->regs.l = (uint8_t)_val;})		\

// We have a special setter for the F register since
// the bottom four bits must always read zero
#define set_F(z80, val)({			\
  uint8_t _val = (val);				\
  z80->regs.f = _val & (_val << 4);})		\

// These ones are just for the sake of consistency
#define set_SP(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.sp = _val;})			\

#define set_PC(z80, val)({			\
      uint16_t _val = (val);			\
      z80->regs.pc = _val;})			\

#define inc_byte_PC(z80)({			\
      z80->regs.pc += 1;})			\

#define get_AF(z80)				\
  ((uint16_t)(z80->regs.a) << 8) | z80->regs.f	\

#define get_BC(z80)				\
  ((uint16_t)(z80->regs.b) << 8) | z80->regs.c	\

#define get_DE(z80)				\
  ((uint16_t)(z80->regs.d) << 8) | z80->regs.e	\

#define get_HL(z80)				\
  ((uint16_t)(z80->regs.h) << 8) | z80->regs.l	\

#define get_SP(z80) z80->regs.sp

#define get_PC(z80) z80->regs.pc

// Macros for setting/resetting the interrupt enable register
#define set_flag_IE(z80)({						\
      z80->regs.ie = true;})						\

#define reset_flag_IE(z80)({						\
      z80->regs.ie = false;})						\

// Macros for setting/resetting/getting flags in register F

#define set_flag_Z(z80)({						\
      z80->regs.f = z80->regs.f | (1 << 7);})				\

#define set_flag_N(z80)({						\
      z80->regs.f = z80->regs.f | (1 << 6);})				\

#define set_flag_H(z80)({						\
      z80->regs.f = z80->regs.f | (1 << 5);})				\

#define set_flag_C(z80)({						\
      z80->regs.f = z80->regs.f | (1 << 4);})				\

#define reset_flag_Z(z80)({						\
      z80->regs.f = z80->regs.f & ~(1 << 7);})				\

#define reset_flag_N(z80)({						\
      z80->regs.f = z80->regs.f & ~(1 << 6);})				\

#define reset_flag_H(z80)({						\
      z80->regs.f = z80->regs.f & ~(1 << 5);})				\

#define reset_flag_C(z80)({						\
      z80->regs.f = z80->regs.f & ~(1 << 4);})				\

#define get_flag_Z(z80)				\
  ((z80->regs.f & (1 << 7)) >> 7)

#define get_flag_N(z80)				\
  ((z80->regs.f & (1 << 6)) >> 6)

#define get_flag_H(z80)				\
  ((z80->regs.f & (1 << 5)) >> 5)

#define get_flag_C(z80)				\
  ((z80->regs.f & (1 << 4)) >> 4)

#endif /* CPU_H */
