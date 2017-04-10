#ifndef _SEM_H_
#define _SEM_H_
#include "mem.h"

#define NBSEMS 5

int scount(int sem);
int screate(short count);
int sdelete(int sem);
int signal(int sem);
int signaln(int sem, short count);
int sreset(int sem, short count);
int try_wait(int sem);
int wait(int sem);

struct procs_bloques {
    int pid;
    struct procs_bloques *next;
};

struct sem {
    int sid;
    short val;
    struct sem *next;
    struct procs_bloques *p_bloques; // liste des processus bloqués sur sémaphore
};
#endif
