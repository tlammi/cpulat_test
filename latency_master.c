

#include "latency.h"
#include "cmdline.h"
#include "shdsem.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_MEAS_COUNT 10000
#define SLAVE_COUNT 3

// Global variables
struct latency glatencies[MAX_MEAS_COUNT];
sem_t* gmtos_sems[SLAVE_COUNT];
sem_t* gstom_sem;


// Prototypes
int init(const struct cmdlineargs* args);
void do_log(const struct cmdlineargs* args, const struct latency* latencies);


int main(int argc, char** argv){

	struct cmdlineargs args;
	int ret = parsecmdline(&args, argc, argv);
	if(ret < 0){
		exit(1);
	}

	// Init the test system.
	ret = init(&args);
	if(ret < 0){
		exit(2);
	}
	int exec_count = args.exec_count > MAX_MEAS_COUNT ? MAX_MEAS_COUNT : args.exec_count;
	while(exec_count--){
		// Store start time
		setstarttime(&glatencies[args.exec_count-exec_count-1]);
		// Increment semaphore 3 times
        shdsem_post(gmtos_sems[0]);
        shdsem_post(gmtos_sems[1]);
        shdsem_post(gmtos_sems[2]);
		// Wait for another semaphore 3 times
		shdsem_wait(gstom_sem);
		shdsem_wait(gstom_sem);
		shdsem_wait(gstom_sem);
		// Store stop time
		setstoptime(&glatencies[args.exec_count-exec_count-1]);
		calcdiff(&glatencies[args.exec_count-exec_count-1]);
	}

	do_log(&args, glatencies);

}



int init(const struct cmdlineargs* args){
    int i;
    for(i=0;i<SLAVE_COUNT;++i){
        char mtos_sem_name[32];
        sprintf(mtos_sem_name, "%s%i",args->mtos_shmprefix,i);
        gmtos_sems[i] = shdsem_register(mtos_sem_name);
        if(gmtos_sems[i] == NULL) return -1;

    }
	gstom_sem = shdsem_register(args->stom_shmname);

    if(gstom_sem == NULL) return -1;
	else return 0;
}


void do_log(const struct cmdlineargs* args, const struct latency* latencies){
	int i;
    printf("Start Time\t\tStop Time\t\tDelta\n");
	for(i=0; i<args->exec_count;++i){
		printf("%lu\t%ld\t%lu\t%ld\t%lu\t%ld\n",
		latencies[i].start.tv_sec,
		latencies[i].start.tv_nsec,
		latencies[i].stop.tv_sec,
		latencies[i].stop.tv_nsec,
		latencies[i].diff.tv_sec,
		latencies[i].diff.tv_nsec);
	}
}

