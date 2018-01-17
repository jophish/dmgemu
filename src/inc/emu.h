#ifndef EMU_H
#define EMU_H

#include "mmu.h"
#include "debug.h"
#include "rom.h"
#include "cpu.h"

typedef struct emu {
  cpu z80;
  rom gb_rom;
  mmu gb_mmu;
  debugger gb_debugger;
} emu;

// Initializes internal structures of the emulator
void init_emu(emu *emu_p);

#endif /* EMU_H */
