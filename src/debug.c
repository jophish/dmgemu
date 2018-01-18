#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "debug.h"
#include "util.h"
#include "error.h"
#include "emu.h"
#include "mmu.h"
#include "opcodes.h"

int debug_prompt(emu *gb_emu_p) {

  // Keeps track of whether or not we're done showing the prompt
  bool success = false;
  bool shown_inst = false;
  // If the prompt would be shown, first show the previous instruction
  // that was executed
  cpu *z80_p = &(gb_emu_p->z80);
  debugger *dbg_p = &(gb_emu_p->gb_debugger);
  // Don't show the prompt at all if we should be stepping
  if (dbg_p->n > 0) {
    dbg_p->n -= 1;
    success = true;
  }

  // If we've told the debugger to execute until we reach a breakpoint, don't show
  // the prompt unless we've actually reached a breakpoint
  if (dbg_p->run) {
    if (hit_breakpoint(z80_p, dbg_p)) {
      dbg_p->run = false;
    } else {
      success = true;
    }
  }


  while(!success) {
    if (!shown_inst) {
      show_previous_inst(gb_emu_p);
      shown_inst = true;
    }
    show_prompt(z80_p);
    char line[MAX_USER_INPUT_LENGTH];
    if(get_user_input(line, sizeof(line)) == ERR_READ_LINE) {
      return ERR_READ_LINE;
    }
    char * dbg_str = strtok(line, TOK_DELIM);
    char *endptr;
    uint16_t addr;
    int err;
    switch (get_debug_tok(dbg_str)) {

      case (TOK_STEP_INST) :
	// This check makes sure "si" is the only token in the input string
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssi takes no arguments\n", ERR_SPACE);
	  break;
	}
	success = true;
	break;

      case (TOK_STEP_N_INSTS) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%ssn takes a single positive integer argument\n", ERR_SPACE);
	  break;
	}

	int num_steps = (int)strtoimax(dbg_str, &endptr, 10);
	if (num_steps <= 0) {
	  printf("%sPlease enter a positive integer number of steps\n", ERR_SPACE);
	  break;
	}
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssn takes a single argument\n", ERR_SPACE);
	  break;
	}

	dbg_p->n = (uint16_t)num_steps;
	success = true;
	break;

      case (TOK_SET_BP) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%sbp takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sbp takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((err = add_breakpoint(dbg_p, addr)) == ERR_MAX_BREAKPOINTS) {
	  printf("%sMaximum number of breakpoints (%d) already set\n", ERR_SPACE, MAX_BREAKPOINTS);
	  break;
	}

	if (err == ERR_DUPLICATE_BP) {
	  printf("%sBreakpoint already exists for this address (0x%04x)\n", ERR_SPACE, addr);
	  break;
	}

	printf("Breakpoint set at address 0x%04x\n", addr);
	break;

      case (TOK_RM_BP) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%srbp takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter a hexadecimal address (e.g., 0xFF12)\n", ERR_SPACE);
	  break;
	}

	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%srbp takes a single argument\n", ERR_SPACE);
	  break;
	}

	if ((err = remove_breakpoint(dbg_p, addr)) == ERR_INVALID_BP) {
	  printf("%sNo breakpoint defined at address 0x%04x\n", ERR_SPACE, addr);
	  break;
	}

	printf("%sSuccessfully removed breakpoint at address 0x%04x\n", ERR_SPACE, addr);
	break;

      case (TOK_CONTINUE) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%sc takes no arguments\n", ERR_SPACE);
	  break;
	}
	dbg_p->run = true;
	success = true;
	break;
      case (TOK_HELP) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%shelp takes no arguments\n", ERR_SPACE);
	  break;
	}
	printf("%sstep a single instruction: si\n", ERR_SPACE);
	printf("%sstep n instructions: sn n\n", ERR_SPACE);
	printf("%sset a breakpoint at hex address n: bp n (e.g., bp 0x0150)\n", ERR_SPACE);
	printf("%sremove a breakpoint at hex address n: rbp n (e.g., rbp 0x0150)\n", ERR_SPACE);
	printf("%sshow all breakpoints: sbp\n", ERR_SPACE);
	printf("%scontinue execution until next breakpoint: c\n", ERR_SPACE);
	printf("%sshow this help menu: help\n", ERR_SPACE);
	break;

      case (TOK_SHOW_BP) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%ssbp takes no arguments\n", ERR_SPACE);
	  break;
	}
	show_breakpoints(dbg_p);
	break;
      default :
	printf("%sPlease enter a valid command (type 'help' for a list)\n", ERR_SPACE);
	break;
    }
  }
  // We're about to step out of the debugger and execute, so set the previous instruction to the
  // current PC
  dbg_p->prev_inst_addr = get_PC(z80_p);
  return 0;
}



int get_debug_tok(char *buf) {
  if (strcmp(buf, DBG_STR_STEP_INST) == 0) {
    return TOK_STEP_INST;
  }
  if (strcmp(buf, DBG_STR_STEP_N_INSTS) == 0) {
    return TOK_STEP_N_INSTS;
  }
  if (strcmp(buf, DBG_STR_HELP) == 0) {
    return TOK_HELP;
  }
  if (strcmp(buf, DBG_STR_SET_BP) == 0) {
    return TOK_SET_BP;
  }
  if (strcmp(buf, DBG_STR_SHOW_BP) == 0) {
    return TOK_SHOW_BP;
  }
  if (strcmp(buf, DBG_STR_RM_BP) == 0) {
    return TOK_RM_BP;
  }
  if (strcmp(buf, DBG_STR_CONTINUE) == 0) {
    return TOK_CONTINUE;
  }
  return ERR_INVALID_TOKEN;
}


void show_prompt(cpu *z80_p) {
  printf("dmgdbg (0x%04x) > ", get_PC(z80_p));
}


char *get_next_tok(void) {
  return strtok(NULL, TOK_DELIM);
}


void init_dbg(debugger *dbg_p) {
  dbg_p->n = 0;
  dbg_p->run = false;
  dbg_p->prev_inst_addr = 0;
  dbg_p->breakpoints = malloc(MAX_BREAKPOINTS*sizeof(uint16_t));
  memset(dbg_p->breakpoints, 0, MAX_BREAKPOINTS*sizeof(uint16_t));
}


int add_breakpoint(debugger *dbg_p, uint16_t addr) {
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      return ERR_DUPLICATE_BP;
    }
  }
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == 0) {
      dbg_p->breakpoints[i] = addr;
      return 0;
    }
  }
  return ERR_MAX_BREAKPOINTS;
}


void show_breakpoints(debugger *dbg_p) {
  int present = 0;
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] != 0) {
      present = 1;
      break;
    }
  }
  if (present == 0) {
    printf("%sNo breakpoints currently set\n", ERR_SPACE);
  } else {
    printf("%sBreakpoints at addresses:\n", ERR_SPACE);
    for (int i = 0; i < MAX_BREAKPOINTS; i++) {
      if (dbg_p->breakpoints[i] != 0) {
	printf("%s%s0x%04x\n", ERR_SPACE, ERR_SPACE, dbg_p->breakpoints[i]);
      }
    }
  }
}


int remove_breakpoint(debugger *dbg_p, uint16_t addr) {
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      dbg_p->breakpoints[i] = 0;
      return 0;
    }
  }
  return ERR_INVALID_BP;
}

bool hit_breakpoint(cpu *z80_p, debugger *dbg_p) {
  return check_breakpoint(dbg_p, get_PC(z80_p));
}

bool check_breakpoint(debugger *dbg_p, uint16_t addr) {
  for (int i = 0; i < MAX_BREAKPOINTS; i++) {
    if (dbg_p->breakpoints[i] == addr) {
      return true;
    }
  }
  return false;
}

void show_previous_inst(emu *gb_emu_p) {
  debugger *dbg_p = &(gb_emu_p->gb_debugger);
  uint16_t addr = dbg_p->prev_inst_addr;
  if (addr == 0) {
    return;
  }
  char buf[MAX_BUF_LEN];
  int err = 0;
  if ((err = addr_to_op_str(gb_emu_p, addr, buf, MAX_BUF_LEN)) == ERR_OP_INVALID_OR_NOT_IMPLEMENTED) {
    printf("%sNo string data available for this op\n", ERR_SPACE);
    return;
  }
  if (err == ERR_BUF_LEN) {
    printf("Debugger needs more buffer space to print this op\n");
    exit(0);
  }
  uint16_t op = read_8(gb_emu_p, addr);
  uint16_t inst_len = op_length(op);
  printf("%s[0x%04x] %s%s(%02x", ERR_SPACE, addr, buf, ERR_SPACE, op);
  for (int i = 1; i < inst_len; i++) {
    printf(" %02x", read_8(gb_emu_p, addr+i));
  }
  printf(")\n");
  return;
}
