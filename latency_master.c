/******************************************************************************
 * \brief Master program source code  for Docker container latency test
 *
 * Program takes slave id's as command line arguments. Slaves should be
 * started before master
 *
 * Master posts IPC semaphores for all slaves and waits for its own semaphore
 * posted by slaves and logs the latency. Exactly 3 slaves are exptected.
 *
 * Usage: <binary_name> <slave id 1> <slave id 2> <slave id 3>
 *
 * \author: Toni Lammi, toni.lammi@wapice.com
******************************************************************************/
#include "latency.h"
#include "shdsem.h"
#include "defines.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <time.h>


// Global variables
//struct latency glatencies[MEAS_COUNT];
struct latency glatency;
sem_t* gmtos_sems[MAX_SLAVE_COUNT];
sem_t* gstom_sem;
sem_t* gctrl_sem;
const struct timespec LOOPDELAY_NS = {
    .tv_sec=0,
    .tv_nsec=10000
};

int init(int argc, char** argv){

	if(argc > MAX_SLAVE_COUNT+1){
		printf("Usage: %s <slave id 1> [<slave id 2> "
					 "[<slave id 3> ... [<slave id 100>]]]\n",argv[0]);
		return -1;
	}
	struct sched_param param = {.sched_priority=99};
	if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
		perror("sched_setscheduler");
		return -1;
	}

	// Register semaphores
  int i;
  for(i=0;i<argc-1;++i){
      char mtos_sem_name[32];
      sprintf(mtos_sem_name, "%s%s",MTOS_SEM_PREFIX, argv[i+1]);
      gmtos_sems[i] = shdsem_register(mtos_sem_name);
      if(gmtos_sems[i] == NULL) return -1;
  }
	gstom_sem = shdsem_register(STOM_SEM_NAME);
  if(gstom_sem == NULL) return -1;

	gctrl_sem = shdsem_register(CTRL_SEM_NAME);
	if(gctrl_sem == NULL) return -1;

	return 0;
}

void do_log(const int exec_count, const struct latency* latencies){
	int i;
    printf("Start Time\t\tStop Time\t\tDelta\n");
	for(i=0; i<exec_count;++i){
		printf("%lu\t%ld\t%lu\t%ld\t%lu\t%ld\n",
		latencies[i].start.tv_sec,
		latencies[i].start.tv_nsec,
		latencies[i].stop.tv_sec,
		latencies[i].stop.tv_nsec,
		latencies[i].diff.tv_sec,
		latencies[i].diff.tv_nsec);
	}
}


void do_cleanup(int argc, char** argv){
	// Increment control semaphore to let slaves exit cleanly
	int i;
	for(i=0;i<argc-1;i++){
		shdsem_post(gctrl_sem);
	}
	// Unlink semaphores and increment mtos_sems to let slaves cleanly exit
	shdsem_unlink(CTRL_SEM_NAME);
	shdsem_unlink(STOM_SEM_NAME);

	for(i=0; i<argc-1; i++){
		char mtos_sem_name[32];
		if(sprintf(mtos_sem_name, "%s%s", MTOS_SEM_PREFIX,argv[i+1]) < 0){
			return;
		}
		shdsem_unlink(mtos_sem_name);
		shdsem_post(gmtos_sems[i]);
	}

}

int main(int argc, char** argv){

	// Init the test system.
	int ret = init(argc, argv);
	if(ret < 0){
		exit(1);
	}
	int exec_count = MEAS_COUNT;

    printf("Start Time\t\tStop Time\t\tDelta\n");

	while(exec_count--){
		// Store start time
		//setstarttime(&glatencies[MEAS_COUNT-exec_count-1]);
        setstarttime(&glatency);
		// Increment semaphore 3 times
		int i=0;
		for(i=0;i<argc-1;i++){
    	shdsem_post(gmtos_sems[i]);
		}
		for(i=0;i<argc-1;i++){
			shdsem_wait(gstom_sem);
		}
		// Store stop time
		//setstoptime(&glatencies[MEAS_COUNT-exec_count-1]);
        setstoptime(&glatency);
		//calcdiff(&glatencies[MEAS_COUNT-exec_count-1]);
        calcdiff(&glatency);
        printf("%lu\t%ld\t%lu\t%ld\t%lu\t%ld\n",
		  glatency.start.tv_sec,
		  glatency.start.tv_nsec,
		  glatency.stop.tv_sec,
		  glatency.stop.tv_nsec,
		  glatency.diff.tv_sec,
		  glatency.diff.tv_nsec);
        nanosleep(&LOOPDELAY_NS, NULL);

	}

	//do_log(MEAS_COUNT, glatencies);
	do_cleanup(argc, argv);

}
