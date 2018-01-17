#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
// Forward declaration of emu typedef to avoid circular include
typedef struct emu emu;

typedef struct debugger {
  uint16_t *breakpoints;
} debugger;

void debug_prompt(emu *gb_emu_p);
#endif /* DEBUG_H */
