#ifndef DEBUG_H
#define DEBUG_H

#include "cpu.h"
#include <stdint.h>

// Forward declarations of typedefs to avoid circular include
typedef struct emu emu;
typedef struct opcode opcode;

typedef struct debugger {
  uint16_t *breakpoints; // List of breakpoints
  uint16_t n; // Number of instructions to step before dropping back into dbg
  bool run;   // Flag indicating whether or not we're running till we hit a breakpoint
  uint16_t prev_inst_addr; // Address of last instruction executed before entering debugger
} debugger;

// Displays the debug prompt and waits for input if appropriate, otherwise
// returns control back to the caller (if for example, we should be executing
// until a breakpoint).
int debug_prompt(emu *gb_emu_p);

// Given a string, returns the debug token it corresponds to. If it isn't
// a valid debugger command, then returns ERR_INVALID_TOKEN
int get_debug_tok(char *buf);

// Just shows the prompt
void show_prompt(cpu *z80_p);

// If strtok has already been called on a string, gets the next token from that string
char *get_next_tok(void);

// Initializes the debugger
void init_dbg(debugger *dbg_p);

// Adds a breakpoint to the list of breakpoints in the debugger if possible.
// If there's no more room in the list, returns ERR_MAX_BREAKPOINTS.
// If the breakpoint is already in the list, returns ERR_DUPLICATE_BP
int add_breakpoint(debugger *dbg_p, uint16_t addr);

// Given a memory address removes the breakpoint associated with that address.
// If the given address does not exist as a breakpoint, returns ERR_INVALID_BP.
int remove_breakpoint(debugger *dbg_p, uint16_t addr);

// Displays the current breakpoints in the debugger prompt
void show_breakpoints(debugger *dbg_p);

// Returns true if the current PC matches a set breakpoint, false otherwise
bool hit_breakpoint(cpu *z80_p, debugger *dbg_p);

// Checks if addr matches any set breakpoints
bool check_breakpoint(debugger *dbg_p, uint16_t addr);

// Displays the previous instruction executed, both in binary and in assembly
void show_previous_inst(emu *gb_emu_p);

// Displays the instruction at the given address, both in binary and in assembly
void show_inst_at_addr(emu *gb_emu_p, uint16_t addr, opcode *op_struct_p);

enum dbg_tok {
  TOK_STEP_INST,
  TOK_STEP_N_INSTS,
  TOK_SET_BP,
  TOK_RM_BP,
  TOK_SHOW_BP,
  TOK_CONTINUE,
  TOK_HELP,
  TOK_READ_MEM_ADDR,
  TOK_READ_MEM_RANGE,
  TOK_SHOW_REGS,
  TOK_SHOW_FLAGS,
  TOK_READ_MEM_ADDR_INST,
  TOK_READ_MEM_INST_RANGE,
};

// Token delimiter
#define TOK_DELIM " \n"
#define ERR_SPACE "     "

#define MAX_USER_INPUT_LENGTH 256
#define MAX_BREAKPOINTS 32
#define MAX_BUF_LEN 100

// Debugger strings
#define DBG_STR_STEP_INST "si"
#define DBG_STR_STEP_N_INSTS "sn"
#define DBG_STR_SET_BP "bp"
#define DBG_STR_RM_BP "rbp"
#define DBG_STR_SHOW_BP "sbp"
#define DBG_STR_CONTINUE "c"
#define DBG_STR_READ_MEM_ADDR "m"
#define DBG_STR_READ_MEM_RANGE "mr"
#define DBG_STR_READ_MEM_ADDR_INST "mi"
#define DBG_STR_READ_MEM_INST_RANGE "mri"
#define DBG_STR_SHOW_REGS "r"
#define DBG_STR_SHOW_FLAGS "f"
#define DBG_STR_HELP "help"

#endif /* DEBUG_H */
