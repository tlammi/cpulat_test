#ifndef LATENCY_H
#define LATENCY_H

#include <time.h>


#define LATENCY_CLOCK CLOCK_MONOTONIC

struct latency{
	struct timespec start;
	struct timespec stop;
	struct timespec diff;
};

void setstarttime(struct latency* lat);
void setstoptime(struct latency* lat);
int calcdiff(struct latency* lat);


#endif
