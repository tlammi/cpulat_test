#include "shdsem.h"

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define OPEN_FLAGS_DEFAULT ((O_CREAT) | (O_EXCL))
#define OPEN_FLAGS_ALT 0
#define MODE_FLAGS ((S_IRWXU)|(S_IRWXG)|(S_IRWXO))

sem_t* shdsem_register(const char* name){
    sem_t* ret_sem = sem_open(name, OPEN_FLAGS_DEFAULT, MODE_FLAGS, 0);
    if(ret_sem == SEM_FAILED) {
        if(errno == EEXIST){
			ret_sem = sem_open(name, OPEN_FLAGS_ALT);
			if(ret_sem == SEM_FAILED) goto error;

		}
		else goto error;
	}

	return ret_sem;

error:
	perror("sem_open()");
	return NULL;
}

int shdsem_unregister(sem_t* shdsem){
    return sem_close(shdsem);
}

int shdsem_unlink(const char* sem_name){
    return sem_unlink(sem_name);
}

int shdsem_post(sem_t* shdsem){
	return sem_post(shdsem);
}
int shdsem_wait(sem_t* shdsem){

	return sem_wait(shdsem);

}


int shdsem_trywait(sem_t* shdsem){
  return sem_trywait(shdsem);
}
