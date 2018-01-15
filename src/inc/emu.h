#ifndef EMU_H
#define EMU_H

#include "rom.h"
#include "cpu.h"

typedef struct emu {
  cpu z80;
  rom gb_rom;
} emu;


#endif /* EMU_H */
