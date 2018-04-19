/******************************************************************************
 * \brief Slave program source code  for Docker container latency test
 *
 * Program takes slave id's as command line arguments,
 * takes the first ID for itself and forks to childs for remaining IDs
 *
 * Each process waits for its own master_to_child_semaphore
 * and increments child_to_master_semaphore.
 *
 * Usage: <binary_name> <slave id 1> <slave id 2> ... <slave id n>
 *
 * \author: Toni Lammi, toni.lammi@wapice.com
******************************************************************************/
#include "shdsem.h"
#include "defines.h"

#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

// Shared semaphores
sem_t* gmtos_sem;
sem_t* gstom_sem;
sem_t* gctrl_sem;

// Child process pids
pid_t gchild_pids[SLAVE_COUNT-1];
// Slave id used by process
char* gslave_id;

/**
 * Forks slave process into multiple processes.
 *
 * Parent process is given the first slave id and next ones are distributed
 * for children
*/
void do_forking(int process_count, char** ids){

  gslave_id = ids[0];
  int i = 0;
  while(i < process_count-1){
    pid_t tmp_pid = fork();
    // Executed for parent
    if(tmp_pid != 0){
      printf("Forked child %d\n",tmp_pid);
      gchild_pids[i] = tmp_pid;
    }
    // Executed for child
    else{
      gslave_id = ids[i+1];
      break;
    }

    i++;
  }
}

/**
 * \brief Initializes shared semaphores and sets scheduling to SCHED_FIFO
*/
int init(const char* id){

  char mtos_sem_name[100];
  if(sprintf(mtos_sem_name, "%s%s",MTOS_SEM_PREFIX,id) < 0){
    return -1;
  }

  gmtos_sem = shdsem_register(mtos_sem_name);
  gstom_sem = shdsem_register(STOM_SEM_NAME);
  gctrl_sem = shdsem_register(CTRL_SEM_NAME);

	if(gmtos_sem == NULL || gstom_sem == NULL || gctrl_sem == NULL) return -1;

	// Set RT scheduling
	struct sched_param param = {.sched_priority=99};
	if(sched_setscheduler(0, SCHED_FIFO, &param) != 0){
    perror("sched_setscheduler");
		return -1;
	}

	return 0;
}


int main(int argc, char** argv){

  printf("Forking...\n");
  do_forking(argc-1, &argv[1]);

  printf("Initializing...\n");
	int ret = init(gslave_id);

  if(ret != 0){
    printf("Init failed\n");
    exit(1);
  }
  printf("Executing:\n");
  while(1){
    shdsem_wait(gmtos_sem);
    // Exit if ctrl semaphore has been posted
    if(!shdsem_trywait(gctrl_sem)){
      break;
    }
    shdsem_post(gstom_sem);
	}

}
