#include "latency.h"

#include <stdio.h>


#define BILLION 1000000000

/**
 * \brief Register start time to latency struct
*/
void setstarttime(struct latency* lat){
    int retval = clock_gettime(LATENCY_CLOCK, &lat->start);
    if(retval){
		lat->start.tv_sec = 0;
		lat->start.tv_nsec = 0;
		perror("clock_gettime()");
	}
}

/**
 * \brief Register stop time for latency struct
*/
void setstoptime(struct latency* lat){
    int retval = clock_gettime(LATENCY_CLOCK, &lat->stop);
    if(retval){
		lat->stop.tv_sec = 0;
		lat->stop.tv_nsec = 0;
		perror("clock_gettime()");
	}
}

/**
\brief Subtracts operand2 from operand1

\return <0 if operand2 is larger, >0 if operand1 is larger, 0 if equal
*/
static inline int subtract_timespec(struct timespec* res,
				const struct timespec* operand1,
				const struct timespec* operand2){

	int retval = 0;
	
	// If operand1 is larger
	if(operand1->tv_sec > operand2->tv_sec){
		res->tv_sec = operand1->tv_sec - operand2->tv_sec;
		res->tv_nsec = operand1->tv_nsec - operand2->tv_nsec;
	
		if(res->tv_nsec <0){
			res->tv_sec--;
			res->tv_nsec += BILLION;
		}
		retval = 1;
	}
	// If operand2 is larger
	else if(operand1->tv_sec < operand2->tv_sec){
		res->tv_sec = operand2->tv_sec - operand1->tv_sec;
		res->tv_nsec = operand2->tv_nsec - operand1->tv_nsec;

		if(res->tv_nsec < 0){
			res->tv_sec--;
			res->tv_nsec += BILLION;
		}
		retval = -1;
	}
	// tv_sec fields are equal
	else{
		res->tv_sec = 0;
		if(operand1->tv_nsec > operand2->tv_nsec){
			res->tv_nsec = operand1->tv_nsec - operand2->tv_nsec;
			retval = 1;
		}
		else if(operand1->tv_nsec < operand2->tv_nsec){
			res->tv_nsec = operand2->tv_nsec - operand1->tv_nsec;
            retval = -1;
		}
		else{
			res->tv_nsec = 0;
			retval = 0;
		}
	}
	return retval;
}


/**
 * \brief Calculates and stores latency
*/
int calcdiff(struct latency* lat){
	return subtract_timespec(&lat->diff, &lat->stop, &lat->start);
}
