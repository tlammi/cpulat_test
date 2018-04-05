#ifndef CMDLINE_H
#define CMDLINE_H

#define SHM_MAX_SIZE 32

struct cmdlineargs{
	int exec_count;
    char mtos_shmprefix[SHM_MAX_SIZE];
	char stom_shmname[SHM_MAX_SIZE];
};


int parsecmdline(struct cmdlineargs* args, int argc, char** argv);


#endif
