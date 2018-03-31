#include "int.h"
#include "mmu.h"
#include <stdlib.h>

int handle_interrupts(emu *gb_emu_p) {
  cpu *z80_p = &(gb_emu_p->z80);

  // If IME isn't set, we don't have to do anything
  if (z80_p->regs.ime == 0)
    return 0;


  uint8_t if_val = read_8(gb_emu_p, REG_IF);
  uint8_t ie_val = read_8(gb_emu_p, REG_IE);
  uint8_t int_and_val = if_val & ie_val;
  // If no interrupts have been requested that are also enabled, just end
  if (int_and_val == 0)
    return 0;


  uint16_t int_vector = 0;

  // Find the interrupt to service
  if ((int_and_val & INT_V_BLANK) != 0) {
    write_8(gb_emu_p, REG_IF, if_val & ~(INT_V_BLANK));
    int_vector = INT_ADDR_V_BLANK;
  }

  // Push the current PC
  uint16_t sp = get_SP(z80_p);
  int err;
  if ((err = write_8(gb_emu_p, sp-1, (uint8_t)(get_PC(z80_p) >> 8))) < 0)
    return err;
  if ((err = write_8(gb_emu_p, sp-2, (uint8_t)(get_PC(z80_p) & 0xFF))) < 0)
    return err;
  set_SP(z80_p, sp - 2);

  // Reset IME
  z80_p->regs.ime = 0;

  if (int_vector == 0)
    exit(1);

  // Set PC and return
  set_PC(z80_p, int_vector);

  return 0;
}
