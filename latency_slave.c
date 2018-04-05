#include "shdsem.h"
#include "cmdline.h"

#include <stdlib.h>

sem_t* gmtos_sem;
sem_t* gstom_sem;

int init(const struct cmdlineargs* args);
void sig_handler(int signo);

int main(int argc, char** argv){
	struct cmdlineargs args;
	int ret = parsecmdline(&args, argc, argv);
	if(ret < 0){
		exit(1);
	}
	ret = init(&args);
	if(ret < 0){
		exit(2);
    }
    while(1){
        shdsem_wait(gmtos_sem);
        shdsem_post(gstom_sem);
	}

	
}


int init(const struct cmdlineargs* args){
    gmtos_sem = shdsem_register(args->mtos_shmprefix);
    gstom_sem = shdsem_register(args->stom_shmname);

	if(gmtos_sem == NULL || gstom_sem == NULL) return -1;
	else return 0;
}



