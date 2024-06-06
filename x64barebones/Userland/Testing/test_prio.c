#include <stdint.h>
#include "test_util.h"
#include "../User/include/userLibAsm.h"
#include "../User/include/userLib.h"

#define MINOR_WAIT "15000000" // TODO: Change this value to prevent a process from flooding the screen
#define WAIT 100000000      // TODO: Change this value to make the wait long enough to see theese processes beeing run at least twice

#define TOTAL_PROCESSES 3
#define LOWEST 0  // TODO: Change as required
#define MEDIUM 2  // TODO: Change as required
#define HIGHEST 3 // TODO: Change as required

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {MINOR_WAIT, 0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++)
    pids[i] = sys_fork(endless_loop_print, argv,"endless_loop_print", 0);

  bussy_wait(WAIT);
  print("\nCHANGING PRIORITIES...\n", 0xffffff, 2);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], prio[i]);

  bussy_wait(WAIT);
  print("\nBLOCKING...\n", 0xffffff, 2);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_block(pids[i]);

  print("CHANGING PRIORITIES WHILE BLOCKED...\n", 0xffffff, 2);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_nice(pids[i], MEDIUM);

  print("UNBLOCKING...\n", 0xffffff, 2);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_unblock(pids[i]);

  bussy_wait(WAIT);
  print("\nKILLING...\n", 0xffffff, 2);

  for (i = 0; i < TOTAL_PROCESSES; i++)
    sys_kill(pids[i]);
}
