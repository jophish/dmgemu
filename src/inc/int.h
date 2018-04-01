#ifndef INT_H
#define INT_H

#include "emu.h"

#define INT_V_BLANK 0x1
#define INT_LCDC 0x2
#define INT_TIMER 0x4
#define INT_SERIAL 0x8
#define INT_JOYPAD 0x16

#define INT_ADDR_V_BLANK 0x40
#define INT_ADDR_LCDC 0x48
#define INT_ADDR_TIMER 0x50
#define INT_ADDR_SERIAL 0x58
#define INT_ADDR_JOYPAD 0x60

// During each instruction/GPU update, some interrupts may be triggered. When an interrupt is
// triggered, we add it to a list of interrupts that have been triggered during this "cycle".
// During normal emulation, when one of these interrupts are triggered, we immediately write the
// appropriate IF bit.

// In our interrupt handling routine, we do the following:
// If any IF bits are set AND IME is set:
//   - out of all set IF bits, get the one with highest interrupt priority
//   - get the interrupt vector address
//   - push current PC on stack
//   - reset the corresponding IF bit
//   - reset IME
//   - set the PC to the interrupt handler

// Handles interrupts with correct priority
int handle_interrupts(emu *gb_emu_p);

#endif /* INT_H */
