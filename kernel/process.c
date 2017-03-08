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
	printf("__________\n\n");
}

void ordonnance() {
	// Wake up proces

	struct process* head;
	display_list(&process_list);
	while ((head = queue_out(&sleeping_list, struct process, links)) != NULL && head->wakeUpTime <= nbr_secondes()) {
		struct process* toWakeUp;
		toWakeUp = head;
		queue_add(toWakeUp, &process_list, struct process, links, prio);
		toWakeUp->state = ACTIVABLE;
		toWakeUp->wakeUpTime = -1;
	}

	// If there is something to activate
	struct process* toChoose = queue_out(&process_list, struct process, links);
	printf("Choosing %s...\n", toChoose->name);
	if (toChoose != NULL) {
		struct process* formerChosen;
		struct process* newChosen;

		// Insert chosen in tail of activable if not sleeping
		if (chosen->state == CHOSEN) {
			chosen->state = ACTIVABLE;
			queue_add(chosen, &process_list, struct process, links, prio);
		}
		formerChosen = chosen;
		newChosen = toChoose;

		chosen = toChoose;
		chosen->state = CHOSEN;

		ctx_sw(formerChosen->register_save, newChosen->register_save);
	}
}

int32_t nbr_secondes() {
	return (int32_t)time;
}

void insertSleep(struct process* process) {
	queue_add(process, &sleeping_list, struct process, links, wakeUpTime);
}

void dors(int nbr_secs) {
	chosen->state = SLEEPING;
	chosen->wakeUpTime = nbr_secondes() + nbr_secs;
	insertSleep(chosen);
	ordonnance();
}

char* mon_nom() {
	return chosen->name;
}

int mon_pid() {
	return chosen->pid;
}

int32_t cree_processus(void (*code)(void), char *nom) {
	pidmax++;
	if (pidmax < PROCESS_TABLE_SIZE) {
		struct process* newprocess = (struct process*)malloc(sizeof(struct process));

		//if (process_list_tail == NULL || process_list_head == NULL) {
		if (queue_empty(&process_list)) {
			newprocess->state = CHOSEN;
			// process_list_tail = newprocess;
			// process_list_head = newprocess;
		} else {
			newprocess->state = ACTIVABLE;
			// process_list_tail->next = newprocess;
			// process_list_tail = newprocess;
		}

		newprocess->pid = pidmax;
		strcpy(newprocess->name, nom);
		newprocess->process_stack[STACK_SIZE -1] = (int)code;
		newprocess->register_save[1] = (int)&(newprocess->process_stack[STACK_SIZE - 1]);
		newprocess->wakeUpTime = -1;

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
	unsigned long i;
	printf("%d", (int)arg);
	while (1) {
		printf("I am in tstA\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (i = 0; i < 5000000; i++);
		ordonnance();
	}
}

int tstB(void *arg)
{
	unsigned long i;
	printf("%d", (int)arg);
	while (1) {
		printf("I am in tstB\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (i = 0; i < 5000000; i++);
		ordonnance();
	}
}

int tstC(void *arg)
{
	unsigned long i;
	printf("%d", (int)arg);
	while (1) {
		printf("I am in tstC\n"); /* l'autre processus doit afficher des 'A' */
		/* boucle d'attente pour ne pas afficher trop de caractères */
		for (i = 0; i < 5000000; i++);
		ordonnance();
	}
}

void init_process_stack(void) {

	cree_processus((void*)&tstA, "dumb_A");
	cree_processus((void*)&tstB, "dumb_B");
	cree_processus((void*)&tstC, "dumb_C");

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
