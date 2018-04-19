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



// Global variables
struct latency glatencies[MEAS_COUNT];
sem_t* gmtos_sems[SLAVE_COUNT];
sem_t* gstom_sem;
sem_t* gctrl_sem;


// Prototypes
int init(int argc, char** argv){

	if(argc != SLAVE_COUNT+1){
		printf("Usage: %s <slave id 1> <slave id 2> <slave id 3>\n",argv[0]);
		return -1;
	}
	struct sched_param param = {.sched_priority=99};
	if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
		perror("sched_setscheduler");
		return -1;
	}

	// Register semaphores
  int i;
  for(i=0;i<SLAVE_COUNT;++i){
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


void do_cleanup(char** argv){
	// Increment control semaphore to let slaves exit cleanly
	shdsem_post(gctrl_sem);
	shdsem_post(gctrl_sem);
	shdsem_post(gctrl_sem);

	// Unlink semaphores and increment mtos_sems to let slaves cleanly exit
	shdsem_unlink(CTRL_SEM_NAME);
	shdsem_unlink(STOM_SEM_NAME);

	int i;

	for(i=0; i<SLAVE_COUNT; i++){
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
	while(exec_count--){
		// Store start time
		setstarttime(&glatencies[MEAS_COUNT-exec_count-1]);
		// Increment semaphore 3 times
    shdsem_post(gmtos_sems[0]);
    shdsem_post(gmtos_sems[1]);
    shdsem_post(gmtos_sems[2]);
		// Wait for another semaphore 3 times
		shdsem_wait(gstom_sem);
		shdsem_wait(gstom_sem);
		shdsem_wait(gstom_sem);
		// Store stop time
		setstoptime(&glatencies[MEAS_COUNT-exec_count-1]);
		calcdiff(&glatencies[MEAS_COUNT-exec_count-1]);
	}

	do_log(MEAS_COUNT, glatencies);
	do_cleanup(argv);

}
