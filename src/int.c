#include "int.h"
#include "mmu.h"

int handle_interrupts(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);
  // If IME isn't set, we don't have to do anything
  if (z80_p->regs.ime == 0)
    return 0;

  mmu *gb_mmu_p = &(gb_emu_p->gb_mmu);
  uint8_t if_val = read_8(gb_emu_p, REG_IF);
  uint8_t ie_val = read_8(gb_emu_p, REG_IE);
  return 0;
}
