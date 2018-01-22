#ifndef GPU_H
#define GPU_H

#include <stdint.h>

// Forward declaration of emu
typedef struct emu emu;

typedef struct gpu_regs {
  uint8_t reg_lcdc,
    reg_stat,
    reg_scy,
    reg_scx,
    reg_ly,
    reg_lyc,
    reg_wy,
    reg_wx,
    reg_bgp,
    reg_obp0,
    reg_obp1,
    reg_dma;
} gpu_regs;

typedef struct gpu {
  uint8_t mode;
  uint32_t gpu_clock;
  uint16_t line;
  gpu_regs gb_gpu_regs;
} gpu;

// Steps the GPU
int step_gpu(emu *gb_emu_p);

// Initializes the GPU with default values
void init_gpu(gpu *gb_gpu_p);

// Writes val to the GPU register at the given address. If the given address
// doesn't correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int write_gpu_reg(gpu *gb_gpu_p, uint16_t addr, uint8_t val);

// Reads val from the GPU register at the given address. If the given address doesn't
// correspond to a valid GPU register, returns ERR_INVALID_ADDRESS.
int read_gpu_reg(gpu *gb_gpu_p, uint16_t addr);

#endif /* GPU_H */
