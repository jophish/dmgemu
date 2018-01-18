#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "debug.h"
#include "util.h"
#include "error.h"
#include "emu.h"

int debug_prompt(emu *gb_emu_p) {
  // If the prompt would be shown, first show the previous instruction
  // that was executed
  cpu *z80_p = &(gb_emu_p->z80);
  debugger *dbg_p = &(gb_emu_p->gb_debugger);
  // Don't show the prompt at all if we should be stepping
  if (dbg_p->n > 0) {
    dbg_p->n -= 1;
    return 0;
  }
  while(1) {

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
	return 0;
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
	return 0;

      case (TOK_SET_BP) :
	if ((dbg_str = get_next_tok()) == NULL) {
	  printf("%sbp takes a single hexadecimal argument\n", ERR_SPACE);
	  break;
	}

	addr = (uint16_t)strtoimax(dbg_str, &endptr, 0);

	if (addr <= 0) {
	  printf("%sPlease enter hexadecimal number of steps (e.g., 0xFF12)\n", ERR_SPACE);
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

      case (TOK_HELP) :
	if ((dbg_str = get_next_tok()) != NULL) {
	  printf("%shelp takes no arguments\n", ERR_SPACE);
	  break;
	}
	printf("%sstep a single instruction: si\n", ERR_SPACE);
	printf("%sstep n instructions: sn n\n", ERR_SPACE);
	printf("%sset a breakpoint at hex address n: bp n (e.g., bp 0x0150)\n", ERR_SPACE);
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
