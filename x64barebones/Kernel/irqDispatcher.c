// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "include/console.h"
#include "include/keyboardDriver.h"

#include "include/time.h"

static void int_20();
static void int_21();

void irqDispatcher(uint64_t irq) {
  switch (irq) {
  case 0:
    int_20();
    break;
  case 1:
    int_21();
    break;
  }
  return;
}

void int_20() {
  timer_handler();
  //  if(ticks_elapsed()%3==0){
  //     return interesting_handler;
  //}
}

void int_21() { keyboardHandler(); }
