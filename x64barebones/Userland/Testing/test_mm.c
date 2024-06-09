#include "test_util.h"
#include <stdint.h>
#include "../User/include/userLibAsm.h"
#include "../User/include/userLib.h"

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1) {
    rq = 0;
    total = 0;

    // Request as many blocks as we can
        while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
        // print("aca estoy RESERVANDO: ", 0xffffff, 2);
        // char sizeStr[15];
        // uintToBase(mm_rqs[rq].size, sizeStr, 10);
        // print(sizeStr, 0xffff00, 2);
        // print("\n", 0xffffff, 2);
        // char totalStr[15];
      mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size);

      if (mm_rqs[rq].address) {
          total += mm_rqs[rq].size;
          // print("mem has been allocated\n", 0xffffff, 2);
          rq++;
      }
            // uintToBase(total, totalStr, 10);
            // print(totalStr, 0x00ff00, 2);
            // print("\n", 0xffffff, 2);
    }
//      char rqstr[4];
//       uintToBase(rq, rqstr, 10);
//       print("SET with rq: \n", 0xffffff, 2);
//       print(rqstr, 0xff0000, 2);
    // Set
    uint32_t i;
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address)
        sys_memset(mm_rqs[i].address, i, mm_rqs[i].size);
    }


    // print("CHECK\n", 0xffffff, 2);

    // Check
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          print("test_mm ERROR\n", 0xffffff, 2);
          return -1;
        }
    }

      // print("FREE\n", 0xffffff, 2);

    // Free
    for (i = 0; i < rq; i++){
      if (mm_rqs[i].address) {
          // print("mem has been freed\n", 0xffffff, 2);
          sys_free(mm_rqs[i].address);
      }
    }
    print("sigo corriendo\n", 0xffffff, 2);
  }
}