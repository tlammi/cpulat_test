#ifndef SHDSEM_H
#define SHDSEM_H

#include <semaphore.h>

sem_t* shdsem_register(const char* name);
int shdsem_unregister(sem_t* shdsem);

int shdsem_unlink(const char* sem_name);

int shdsem_post(sem_t* shdsem);
int shdsem_wait(sem_t* shdsem);
int shdsem_trywait(sem_t* shdsem);

#endif
