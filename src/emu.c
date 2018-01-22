#include "emu.h"

void init_emu(emu *emu_p) {
  init_cpu(&(emu_p->z80));
  init_mmu(&(emu_p->gb_mmu));
  init_gpu(&(emu_p->gb_gpu));
  init_dbg(&(emu_p->gb_debugger));
}
