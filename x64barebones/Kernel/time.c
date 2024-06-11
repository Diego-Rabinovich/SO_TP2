#include "include/time.h"

#include "scheduler.h"
#include "videoDriver.h"
static unsigned long ticks = 0;


int ticks_elapsed() {
	return ticks;
}

int seconds_elapsed() {
	return ticks / 18;
}

uint64_t millis_elapsed() {
	return ticks*56;
}

void timer_handler() {
	ticks++;
}