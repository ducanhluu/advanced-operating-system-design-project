#ifndef __PROCESS_H__
#define __PROCESS_H__

#include "queue.h"
#include <inttypes.h>

#define MAX_NAME_LENGTH 64
#define STACK_SIZE 8192
#define PROCESS_TABLE_SIZE 64 //TODO plutôt NBPROC ?
#define MAXPRIO 256 //TODO verif borné partout

int idle(void *);
void init_process_stack();
void ordonnance();
int32_t nbr_secondes();
void ctx_sw(int* old, int* new);

struct list_link process_list;
struct list_link toKill_list;
struct list_link sleeping_list;

void display_list(link* head);


void unblock(int pid);
int getpid();
void block_send(int pid);
void block_recv(int pid);

// PRIMITIVES SYSTEMES
int32_t start(int (*code)(void *), const char *nom, unsigned long ssize, int prio, void *arg);
void exit(int retval);
int kill(int pid);
int waitpid(int pid, int *retvalp);
int chprio(int pid, int newprio);
int getprio(int pid);
// SEMAPHORE
void bloque_sur_semaphore();
void passe_activable(int pid);



typedef enum {CHOSEN, ACTIVABLE, SLEEPING, BLOCKED_ON_SEM, BLOCKED_ON_MSG_SEND, BLOCKED_ON_MSG_RCV, ZOMBIE} process_state;

struct children {
    int pid;
    struct children *next;
};

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
  int parent_pid;
  struct children *children;
};

#endif
