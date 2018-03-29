#ifndef INT_H
#define INT_H

#include "emu.h"

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
