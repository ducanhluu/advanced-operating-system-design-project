#ifndef __PROCESS_H__
#define __PROCESS_H__

//#include "../shared/malloc.c"
#include "../shared/inttypes.h"
//#include "message_queue.h"

#define MAX_NAME_LENGTH 64
#define STACK_SIZE 512
#define PROCESS_TABLE_SIZE 5

void idle();
void proc1();
void init_process_stack();
void ordonnance();
int32_t nbr_secondes();
void ctx_sw(int* old, int* new);
void maj_sleeping(int pid);

struct process* process_list_head;
struct process* process_list_tail;

struct process* sleeping_list_head;
struct process* sleeping_list_tail;

typedef enum {CHOSEN, ACTIVABLE, SLEEPING} process_state;

struct process {
	int pid;
	char name[MAX_NAME_LENGTH];
	process_state state;
	int register_save[5];
	int process_stack[STACK_SIZE];
	struct process* next;
	int wakeUpTime;
};

#endif
