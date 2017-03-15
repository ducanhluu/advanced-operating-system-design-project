#include <cpu.h>
//#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "process.h"
#include "console.h"
#include "time.h"
#include "../shared/malloc.c"

LIST_HEAD(process_list);

LIST_HEAD(sleeping_list);

struct process* chosen = NULL;

int pidmax = -1;

void display_list(link* head) {
	printf("DISPLAY\n");
	if (chosen != NULL) {
		printf("chosen : \n");
		printf("%s [%d]\n", chosen->name, chosen->prio);
	}
	printf("process_list : \n");
	struct process* cour;
	queue_for_each(cour, head, struct process, links) {
		printf("%s [%d]\n", cour->name, cour->prio);
	}
	printf("head : \n");
	if (!queue_empty(&process_list))
	printf("%s\n",((struct process*)queue_top(&process_list, struct process, links))->name);
	printf("__________\n\n");
}

void ordonnance() {
	//	display_list(&process_list);
	struct process* toChoose = queue_out(&process_list, struct process, links);
	if (toChoose != NULL) {
		toChoose->state = CHOSEN;
		if (chosen->state == CHOSEN) {
			chosen->state = ACTIVABLE;
			queue_add(chosen, &process_list, struct process, links, prio);
		}
		struct process* former = chosen;
		chosen = toChoose;
		ctx_sw(former->register_save, chosen->register_save);
	}
}

int32_t nbr_secondes() {
	return (int32_t)time;
}

char* mon_nom() {
	return chosen->name;
}

int mon_pid() {
	return chosen->pid;
}

int start (void (*code)(void), const char *nom, unsigned long ssize, int prio, void *arg) {
	//int32_t start(void (*code)(void), char *nom) {
	pidmax++;
	if (pidmax < PROCESS_TABLE_SIZE) {
		struct process* newprocess = (struct process*)mem_alloc(sizeof(struct process));
		
		if (queue_empty(&process_list)) {
			newprocess->state = CHOSEN;
		} else {
			newprocess->state = ACTIVABLE;
		}

		newprocess->pid = pidmax;
		strcpy(newprocess->name, nom);
		newprocess->process_stack = (int*)mem_alloc(ssize * sizeof(int));
		(void)arg;
		//newprocess->process_stack[STACK_SIZE -1] = (int)code;
		newprocess->process_stack[ssize -1] = (int)code;
		newprocess->register_save[1] = (int)&(newprocess->process_stack[STACK_SIZE - 1]);
		newprocess->wakeUpTime = -1;
		newprocess->prio = prio;

		queue_add(newprocess, &process_list, struct process, links, prio);

		return pidmax;
	} else {

		printf("error, cannot insert %s", nom);

		return -1;
	}
}


void idle(void)
{
	for (;;) {
		sti();
		hlt();
		cli();
	}
}

int tstA(void *arg)
{
	(void)arg;
	for (;;) {
		printf("I am in A\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (int i = 0; i < 1000000; i++){};
		
	}
}

int tstB(void *arg)
{
	(void)arg;
	for (;;) {
		printf("I am in B\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (int i = 0; i < 1000000; i++){};
	}
}

int tstC(void *arg)
{
	(void)arg;
	for (;;) {
		printf("I am in C\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (int i = 0; i < 1000000; i++){};
	}
}

int tstD(void *arg)
{
	(void)arg;
	for (;;) {
		printf("I am in D\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (int i = 0; i < 1000000; i++){};
	}
}

void init_process_stack(void) {


	start((void*)&idle, "idle", 1024, 512, NULL);
	start((void*)&tstA, "dumb_A", 1024, 512, NULL);
	start((void*)&tstB, "dumb_B", 1024, 512, NULL);
	start((void*)&tstC, "dumb_C", 1024, 512, NULL);
	start((void*)&tstD, "dumb_D", 1024, 512, NULL);

	display_list(&process_list);

	chosen = queue_out(&process_list, struct process, links);
}

void unblock(int pid) {
  struct process* cour;
	queue_for_each(cour, &process_list, struct process, links) {
		if (cour->pid == pid && (cour->state == BLOCKED_ON_MSG_RCV || cour->state == BLOCKED_ON_MSG_SEND)) {
			cour->state = ACTIVABLE;
			ordonnance();
			return;
		}
	}
	printf("Process %d not blocked\n", pid);
}

void block_send(int pid){
	struct process* cour;
	queue_for_each(cour, &process_list, struct process, links) {
		if (cour->pid == pid) {
			cour->state = BLOCKED_ON_MSG_SEND;
			ordonnance();
		  return;
		}
	}
	printf("Process %d not found\n", pid);
}

void block_recv(int pid){
	struct process* cour;
	queue_for_each(cour, &process_list, struct process, links) {
		if (cour->pid == pid) {
			cour->state = BLOCKED_ON_MSG_RCV;
			ordonnance();
		  return;
		}
	}
	printf("Process %d not found\n", pid);
}


// SEMAPHORE
void bloque_sur_semaphore() {
    chosen->state = BLOCKED_ON_SEM;
    ordonnance();
}

void passe_activable(int pid) {
	struct process* cour;
	queue_for_each(cour, &process_list, struct process, links) {
		if (cour->pid == pid) {
            		cour->state = ACTIVABLE;
            		break;
        	}
	}
}
