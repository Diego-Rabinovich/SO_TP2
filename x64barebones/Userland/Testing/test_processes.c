// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "test_util.h"
#include "../User/include/userLibAsm.h"
#include "../User/include/userLib.h"

enum State { RUNNING,
             BLOCKED,
             KILLED };

typedef struct P_rq {
  int32_t pid;
  enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint64_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1)
    return -1;

  if ((max_processes = satoi(argv[0])) <= 0)
    return -1;

  int16_t fds[3];
  sys_get_FDs(fds);
  fds[0] = DEV_NULL;

  p_rq p_rqs[max_processes];

  while (1) {
    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      p_rqs[rq].pid = sys_createProcess((Main) endless_loop,argvAux,"endless_loop", 0, fds);
      if (p_rqs[rq].pid == -1) {
        print("test_processes: ERROR creating process\n", 0xff0000, 2);
        return -1;
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }
    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {
      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              if (sys_kill(p_rqs[rq].pid) == -1) {
                print("test_processes: ERROR killing process\n", 0xff0000, 2);
                return -1;
              }
                p_rqs[rq].state = KILLED;
              alive--;
            }
            break;
          default:
            if (p_rqs[rq].state == RUNNING) {
              if (sys_block(p_rqs[rq].pid) == -1) {
                print("test_processes: ERROR blocking process\n", 0xff0000, 2);
                return -1;
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }
        // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          if (sys_unblock(p_rqs[rq].pid) == -1) {
            print("test_processes: ERROR unblocking process\n", 0xff0000, 2);
            return -1;
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  }
}
