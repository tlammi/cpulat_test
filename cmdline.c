#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parsecmdline(struct cmdlineargs* args, int argc, char** argv){
	if(argc < 3){
		printf("Usage: %s <master-to-slave-sem> <slave-to-master-sem>\n",argv[0]);
		return -1;
    }
    strcpy(args->mtos_shmprefix,argv[1]);
    strcpy(args->stom_shmname,argv[2]);
    if(argc == 4){
		args->exec_count = atoi(argv[3]);
	}
	else{
		args->exec_count = 0;
    }
	return 0;
}
