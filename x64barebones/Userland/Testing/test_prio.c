#include <stdint.h>

#include "test_util.h"
#include "../User/include/userLibAsm.h"
#include "../User/include/userLib.h"

#define MINOR_WAIT "20000000"
#define WAIT 200000000

#define TOTAL_PROCESSES 3
#define LOWEST 0
#define MEDIUM 2
#define HIGHEST 3

int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};

void test_prio() {
  int64_t pids[TOTAL_PROCESSES];
  char *argv[] = {MINOR_WAIT, 0};
  uint64_t i;

  for (i = 0; i < TOTAL_PROCESSES; i++) {
    int16_t* fds = sys_malloc(3*sizeof(int16_t));
    sys_get_FDs(fds);
    fds[0] = DEV_NULL;

    pids[i] = sys_createProcess(endless_loop_print, argv,"endless_loop_print", 0, fds);
  }

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
