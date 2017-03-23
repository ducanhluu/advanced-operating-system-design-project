#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "queue.h"
#include <inttypes.h>

#define MAX_NAME_LENGTH 64
#define STACK_SIZE 8192
#define PROCESS_TABLE_SIZE 10

int idle(void *);
void proc1();
void init_process_stack();
void ordonnance();
int32_t nbr_secondes();
void ctx_sw(int* old, int* new);

void unblock(int pid);
int mon_pid();
void block_send(int pid);
void block_recv(int pid);

// PRIMITIVES SYSTEMES
int32_t start(int (*code)(void *), const char *nom, unsigned long ssize, int prio, void *arg);
void exit(int retval);

// SEMAPHORE
void bloque_sur_semaphore();
void passe_activable(int pid);


typedef enum {CHOSEN, ACTIVABLE, SLEEPING, BLOCKED_ON_SEM, BLOCKED_ON_MSG_SEND, BLOCKED_ON_MSG_RCV, ZOMBIE} process_state;

struct process {
  int pid;
  int prio;
  link links;
  char name[MAX_NAME_LENGTH];
  process_state state;
  int register_save[5];
  int* process_stack;
  int wakeUpTime;
  int retval;
};

#endif
