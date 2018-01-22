#include "gpu.h"
#include "emu.h"

int step_gpu(emu *gb_emu_p) {
  gpu *gb_gpu_p = &(gb_emu_p->gb_gpu);
  switch (gb_gpu_p->mode) {
    // H-Blank period
    case (0) :
      break;
    // 
    case (1) :
      break;
    case (2) :
      break;
    case (3) :
      break;
  }
  return 0;
}
void init_gpu(gpu *gb_gpu_p) {
  gb_gpu_p->mode = 0;
  gb_gpu_p->gpu_clock = 0;
  gb_gpu_p->line = 0;
}
