#ifndef GPU_H
#define GPU_H

#include <stdint.h>

// Forward declaration of emu
typedef struct emu emu;

typedef struct gpu {
  uint8_t mode;
  uint32_t gpu_clock;
  uint16_t line;
} gpu;

// Steps the GPU
int step_gpu(emu *gb_emu_p);

// Initializes the GPU with default values
void init_gpu(gpu *gb_gpu_p);

#endif /* GPU_H */
