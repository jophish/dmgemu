#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdio.h>
#include "mmu.h"

#define DIV_COUNT_MAX 256
#define CLOCK_0_COUNT_MAX 1024
#define CLOCK_1_COUNT_MAX 16
#define CLOCK_2_COUNT_MAX 64
#define CLOCK_3_COUNT_MAX 256

static const uint16_t TIMER_COUNTS[4] = {CLOCK_0_COUNT_MAX,
				       CLOCK_1_COUNT_MAX,
				       CLOCK_2_COUNT_MAX,
				       CLOCK_3_COUNT_MAX};
// Forward declaration of emu
typedef struct emu emu;

typedef struct timer_regs {
  uint8_t reg_div,
    reg_tima,
    reg_tma,
    reg_tac;
} timer_regs;

typedef struct timer {
  timer_regs gb_timer_regs;
  uint32_t timer_count, div_count;
} timer;

// Writes val to the timer register at the given address. If the given address
// doesn't correspond to a valid timer register, returns ERR_INVALID_ADDRESS.
int write_timer_reg(emu *gb_emu_p, uint16_t addr, uint8_t val);

// Reads val from the timer register at the given address. If the given address doesn't
// correspond to a valid timer register, returns ERR_INVALID_ADDRESS.
int read_timer_reg(timer *gb_timer_p, uint16_t addr);

// Updates the timer based on internal state and triggers an interupt if necessary.
int update_timer(emu *gb_emu_p);

// Initializes the timer
int init_timer(timer *gb_timer_p);

#endif /* TIMER_H */
