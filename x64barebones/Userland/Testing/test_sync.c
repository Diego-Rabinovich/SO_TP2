#include <stdint.h>
#include "test_util.h"
#include "../User/include/userLibAsm.h"
#include "../User/include/userLib.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory

void slowInc(int64_t *p, int64_t inc) {
  int64_t aux = *p; //we made aux int64_t instead of uint64_t so that if a dec goes below 0 global does not go to int max.
  sys_yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
}

uint64_t my_process_inc(uint64_t argc, char *argv[]) {
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return -1;

  if ((n = satoi(argv[0])) <= 0)
    return -1;
  if ((inc = satoi(argv[1])) == 0)
    return -1;
  if ((use_sem = satoi(argv[2])) < 0)
    return -1;

  if (use_sem)
    if (sys_sem_new(SEM_ID, 1) == -1) {
      print("test_sync: ERROR opening semaphore\n", 0xff0000, 2);
      return -1;
    }

  uint64_t i;
  for (i = 0; i < n; i++) {
    if (use_sem)
      sys_sem_wait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      sys_sem_post(SEM_ID);
  }

  return 0;
}

uint64_t test_sync(uint64_t argc, char *argv[]) { //{n, use_sem, 0}
  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];
  int16_t fdsAux[3] = {DEV_NULL, STDOUT, STDERR};

    if (argc != 2)
    return -1;

  char *argvDec[] = {argv[0], "-1", argv[1], 0};
  char *argvInc[] = {argv[0], "1", argv[1], 0};

  global = 0;

  uint64_t i;
  for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
    pids[i] = sys_createProcess((Main) my_process_inc, argvDec,  "my_process_dec", 3, fdsAux);
    pids[i + TOTAL_PAIR_PROCESSES] = sys_createProcess((Main) my_process_inc, argvInc,  "my_process_inc", 0, fdsAux);
  }

  for (i = 0; i < 2 * TOTAL_PAIR_PROCESSES; i++) {
    sys_wait_pid(-1); //we change this to -1 since we do not support zombies
  }

  if (satoi(argv[1]))
        sys_sem_destroy(SEM_ID);

  char globalStr[100];
  uintToBase(global, globalStr, 10);
  print("\nFinal value: ", 0xffffff, 2);
  print(globalStr, 0xffffff, 2);
  print("\n", 0xffffff, 2);

  return 0;
}
