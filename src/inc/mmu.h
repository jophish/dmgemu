#ifndef MMU_H
#define MMU_H

#include <stdint.h>

typedef uint16_t mem_addr;

uint8_t read_8(mem_addr addr);
uint16_t read_16(mem_addr addr);

void write_8(mem_addr addr);
void write_16(mem_addr addr);

#endif /* MMU_H */
