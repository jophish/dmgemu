#include "timer.h"
#include "emu.h"
#include "int.h"

int init_timer(timer *gb_timer_p) {
  gb_timer_p->gb_timer_regs.reg_div = 0;
  gb_timer_p->gb_timer_regs.reg_tima = 0;
  gb_timer_p->gb_timer_regs.reg_tma = 0;
  gb_timer_p->gb_timer_regs.reg_tac = 0;
  gb_timer_p->timer_count = 0;
  gb_timer_p->div_count = 0;
  return 0;
}

int write_timer_reg(emu *gb_emu_p, uint16_t addr, uint8_t val) {
  timer *gb_timer_p = &(gb_emu_p->gb_timer);
  switch (addr) {
  case (REG_DIV):
    gb_timer_p->gb_timer_regs.reg_div = 0;
    break;
  case (REG_TIMA):
    gb_timer_p->gb_timer_regs.reg_tima = val;
    break;
  case (REG_TMA):
    gb_timer_p->gb_timer_regs.reg_tma = val;
    break;
  case (REG_TAC):
    gb_timer_p->gb_timer_regs.reg_tac = val;
    break;
  default :
      return ERR_INVALID_ADDRESS;
  }
  return 0;
}

int read_timer_reg(timer *gb_timer_p, uint16_t addr) {
  switch (addr) {
  case (REG_DIV):
    return gb_timer_p->gb_timer_regs.reg_div;
  case (REG_TIMA):
    return gb_timer_p->gb_timer_regs.reg_tima;
  case (REG_TMA):
    return gb_timer_p->gb_timer_regs.reg_tma;
  case (REG_TAC):
    return gb_timer_p->gb_timer_regs.reg_tac;
  default :
    return ERR_INVALID_ADDRESS;
  }
  return ERR_INVALID_ADDRESS;
}

int update_timer(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  timer *gb_timer_p = &(gb_emu_p->gb_timer);
  mmu *gb_mmu_p = &(gb_emu_p->gb_mmu);

  uint8_t prev_cycles = z80_p->clk.prev_cpu_cycles;
  // increment div count from internal clock
  uint32_t result = gb_timer_p->div_count + prev_cycles;
  gb_timer_p->div_count = result % DIV_COUNT_MAX;

  // if div count has overflowed, increment div
  if (result >= DIV_COUNT_MAX)
    gb_timer_p->gb_timer_regs.reg_div += 1;

  // If timer is stopped (look at TAC), return
  if (gb_timer_p->gb_timer_regs.reg_tac & 0x08)
    return 0;
  uint16_t timer_max = TIMER_COUNTS[gb_timer_p->gb_timer_regs.reg_tac & 0x3];
  // increment timer count from internal clock
  result = gb_timer_p->timer_count + prev_cycles;
  gb_timer_p->timer_count = result % timer_max;
  // if count has overflowed, increment timer
  if (result >= timer_max) {
    gb_timer_p->gb_timer_regs.reg_tima += 1;
    // if timer has overflowed, reload the timer and request an interrupt
    if (gb_timer_p->gb_timer_regs.reg_tima == 0) {
      // reload timer
      gb_timer_p->gb_timer_regs.reg_tima = gb_timer_p->gb_timer_regs.reg_tma;
      // request interrupt
      gb_mmu_p->hw_io_regs[REG_IF-HW_IO_REGS_START] |= INT_TIMER;
    }
  }
  return 0;
}
