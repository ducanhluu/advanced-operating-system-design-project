#include <cpu.h>
//#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "process.h"
#include "console.h"
#include "time.h"
#include "../shared/malloc.c"

struct process* process_list_head = NULL;
struct process* process_list_tail = NULL;

struct process* sleeping_list_head = NULL;
struct process* sleeping_list_tail = NULL;

struct process* chosen = NULL;

int pidmax = -1;

void display_list(struct process* head) {
	struct process* cour = head;
	while(cour != NULL) {
		printf("%s ", cour->name);
		cour = cour->next;
	}
	printf("\n");
}

void insert(struct process* p, struct process** head, struct process** tail) {
	if (*head == NULL || *tail == NULL) {
		*head = p;
		*tail = p;
	} else {
		(*tail)->next = p;
		p->next = NULL;
		*tail = p;
	}
}

struct process* pick(struct process** head, struct process** tail) {
	if (*head == NULL || *tail == NULL) {
		return NULL;
	}

	struct process* res = *head;
	*head = (*head)->next;
	if (*head == NULL) {
		*tail = NULL;
	}
	res->next = NULL;
	return res;
}

void ordonnance() {
	// Wake up proces
	while(sleeping_list_head != NULL && sleeping_list_head->wakeUpTime <= nbr_secondes()) {
		struct process* toWakeUp;
		toWakeUp = pick(&sleeping_list_head, &sleeping_list_tail);
		insert(
			toWakeUp,
			&process_list_head,
			&process_list_tail);
		toWakeUp->state = ACTIVABLE;
		toWakeUp->wakeUpTime = -1;
	}

	// If there is something to activate
	struct process* toChoose = pick(&process_list_head, &process_list_tail);
	if (toChoose != NULL) {
		struct process* formerChosen;
		struct process* newChosen;

		// Insert chosen in tail of activable if not sleeping
		if (chosen->state == CHOSEN) {
			chosen->state = ACTIVABLE;
			insert(
				chosen,
				&process_list_head,
				&process_list_tail);
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
	// If list empty
	if (sleeping_list_head == NULL || sleeping_list_tail == NULL) {
		sleeping_list_head = process;
		sleeping_list_tail = process;
		return;
	}
	if (process->wakeUpTime <= sleeping_list_head->wakeUpTime) {
		process->next = sleeping_list_head;
		sleeping_list_head = process;
		return;
	}
	if (sleeping_list_tail->wakeUpTime <= process->wakeUpTime) {
		sleeping_list_tail->next = process;
		sleeping_list_tail = process;
		return;
	}

	// insert at the right place
	struct process* cour = sleeping_list_head;
	struct process* prec;
	while(cour->next != NULL && cour->wakeUpTime < process->wakeUpTime) {
		prec = cour;
		cour = cour->next;
	}
	prec->next = process;
	process->next = cour;
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

		if (process_list_tail == NULL || process_list_head == NULL) {
			newprocess->state = CHOSEN;
			process_list_tail = newprocess;
			process_list_head = newprocess;
		} else {
			newprocess->state = ACTIVABLE;
			process_list_tail->next = newprocess;
			process_list_tail = newprocess;
		}
		newprocess->pid = pidmax;
		strcpy(newprocess->name, nom);
		newprocess->process_stack[STACK_SIZE -1] = (int)code;
		newprocess->register_save[1] = (int)&(newprocess->process_stack[STACK_SIZE - 1]);
		newprocess->next = NULL;
		newprocess->wakeUpTime = -1;
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

void proc1(void) {
	for (;;) {
		// printf(__FUNCTION__);
		printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
		dors(2);
	}
}

void proc2(void) {
	for (;;) {
		// printf(__FUNCTION__);
		printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
		dors(3);
	}
}
void proc3(void) {
	for (;;) {
		// printf(__FUNCTION__);
		printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
		dors(5);
	}
}

void init_process_stack(void) {

	cree_processus((void*)&idle, "idle");
	cree_processus((void*)&proc1, "proc1");
	cree_processus((void*)&proc2, "proc2");
	cree_processus((void*)&proc3, "proc3");

	chosen = pick(&process_list_head, &process_list_tail);

}

void maj_sleeping(int pid) {
  //Remet wake up time à zero après réception d'un message
  struct process* cour = sleeping_list_head;
  struct process* proc;
  if (cour == NULL) {
    //erreur
    printf("Process not found in sleeping list\n");
  }
  if (cour->pid == pid) {
    proc = cour;
    sleeping_list_head = cour->next;
    if (cour->next == NULL) {
      sleeping_list_tail = NULL;
    }
  } else {
    while(cour->next != NULL && cour->next->pid!=pid) {
      cour = cour->next;
    }
    if (cour->next == NULL) {
      //erreur
      printf("Process not found in sleeping list\n");
    } 
    proc = cour->next;
    cour->next = proc->next;
    if (proc->next == NULL) {
      sleeping_list_tail = cour;
    }
  }
  //Processus supprimé de la liste sleeping
  proc->wakeUpTime = nbr_secondes();
  insertSleep(chosen);
  ordonnance();
}
