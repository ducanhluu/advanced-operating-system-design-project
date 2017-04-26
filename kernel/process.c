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
LIST_HEAD(toKill_list);

struct process* chosen = NULL;

int pidmax = -1;

void display_list(link* head) {
	printf("DISPLAY\n");
	if (chosen != NULL) {
		printf("## chosen : \n");
		printf("   %s [PID=%d ; PRIO=%d]\n", chosen->name, chosen->pid, chosen->prio);
	}
	printf("## Process_list : \n");
	struct process* cour;
	queue_for_each(cour, head, struct process, links) {
		printf("   %s [PID=%d ; PRIO=%d]\n", cour->name, cour->pid, cour->prio);
	}
	printf("__________\n\n");
}

void ordonnance() {
	// display_list(&process_list);
	// Take the first activable process : we handle zombie process
	
	while (!queue_empty(&process_list) && ((struct process*)queue_top(&process_list, struct process, links))->state == ZOMBIE) {
		struct process* cour = queue_out(&process_list, struct process, links);
		queue_add(cour, &toKill_list, struct process, links, prio);
	}


	struct process* toChoose = queue_out(&process_list, struct process, links);
	
	if (toChoose != NULL) {
		toChoose->state = CHOSEN;
		if (chosen->state == CHOSEN) {
			chosen->state = ACTIVABLE;
			queue_add(chosen, &process_list, struct process, links, prio);
		} else if (chosen->state == SLEEPING) {
			queue_add(chosen, &sleeping_list, struct process, links, prio);
		} else if (chosen->state == ZOMBIE) {
			queue_add(chosen, &toKill_list, struct process, links, prio);
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

int getpid() {
	return chosen->pid;
}

void hdl_ret() {

    register int eax __asm__("eax");
    exit(eax);
}

int32_t start(int (*code)(void *), const char *nom, unsigned long ssize, int prio, void *arg) {
	pidmax++;
	if (pidmax < PROCESS_TABLE_SIZE && prio >= 1 && prio <= MAXPRIO) {
		struct process* newprocess = (struct process*)mem_alloc(sizeof(struct process));
		
		if (queue_empty(&process_list) && chosen == NULL) {
			newprocess->state = CHOSEN;
		} else {
			newprocess->state = ACTIVABLE;
		}

		newprocess->pid = pidmax;
		strcpy(newprocess->name, nom);
		newprocess->process_stack = (int*)mem_alloc(ssize * sizeof(int));

		newprocess->process_stack[ssize - 3] = (int)code;// CODE FONCTION
		newprocess->process_stack[ssize - 2] = (int)hdl_ret; // ADR RETOUR
		newprocess->process_stack[ssize - 1] = (int)arg; // ARG
		newprocess->register_save[1] = (int)&(newprocess->process_stack[ssize - 3]);
		newprocess->wakeUpTime = -1;
		newprocess->prio = prio;
		///////////////////////////////
		/* LIST_HEAD(children); */
		/* newprocess->children = &children; */
		
		/* struct children *child =  (struct children*)mem_alloc(sizeof(struct children)); */
		/* child->pid = newprocess->pid; */
		
		if (chosen != NULL) {
		  //  queue_add(child, chosen->children, struct children, links, pid);
		  ///////////////////////////////
		  newprocess->parent_pid = chosen->pid;
		  queue_add(newprocess, &process_list, struct process, links, prio);
		  if (chosen->prio < prio)
		    ordonnance();
		} else {
		
		  queue_add(newprocess, &process_list, struct process, links, prio);
		}
		
		return pidmax;
	} else {
	        pidmax--;
		printf("error, cannot insert %s", nom);

		return -1;
	}
}


int idle(void *arg)
{
  (void)arg; 
	for (;;) {
		sti();
		hlt();
		cli();
	}
	return 0;
}

void init_process_stack(void) {


	start(idle, "idle", 1024, 1, NULL);

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

// PRIMITIVES SYSTEMES

void exit(int retval) {
   chosen->retval = retval;
   chosen->state = ZOMBIE;
   ordonnance();
  
   for (;;){}
}

/**
 * kill a process with given PID
 * pid : the pid of the process to kill
 * returns : 0 for success, n<0 for fail
 */

int kill(int pid) {
	cli();


  struct process* cour;
  if (pid <= 0) {
    return -1;
  }
  queue_for_each(cour, &process_list, struct process, links) {
    if (cour->pid == pid && cour->state != ZOMBIE) {
      cour->state = ZOMBIE;
      cour->retval = 0;

      sti();
      return 0;
    }
  }
  //process non trouvé
	sti();
  return -1;
}

/**
 * Attend la terminaison d'un fils
 * pid : le pid à attendre
 * retvalp : la valeur de retour du fils
 * returns : 
 *  si pid est negatif, on attend n'importe quel fils
 *  si pid est positif, on renvoit la valeur de retour dans retvalp
 */

int waitpid(int pid, int *retvalp) {
  if (pid < 0) {
    // le processus appelant attend qu'un de ses fils, n'importe lequel, soit terminé et récupère (le cas échéant) sa valeur de retour dans *retvalp, à moins que retvalp soit nul. Cette fonction renvoie une valeur strictement négative si aucun fils n'existe ou sinon le pid de celui dont elle aura récupéré la valeur de retour.
  } else if (pid > 0) {
	  // le processus appelant attend que son fils ayant ce pid soit terminé ou tué
	  struct process* cour = NULL;
	  bool found = false;

	  /* struct children *child; */
	  
	  /* if (queue_empty(chosen->children)) { */
	  /*   printf("%s", chosen->name); */
	  /*   return -1; //ce processus appelant n'a aucun fils */
	  /* } */
	  
	  /* queue_for_each(child, chosen->children, struct children, links) { */
	  /*   if (child->pid == pid) { */
	  /*     found = true; */
	  /*     break; */
	  /*   } */
	  /* } */
	  
	  /* if (!found) { */
	  /*   return -1; //ce pid n'est pas un fils du processus appelant. */
	  /* } */
	  
	  queue_for_each(cour, &process_list, struct process, links) {
		if (cour->pid==pid) {
			found = true;
			break;
		}
		found = false;
	  }

	  //cherche dans la liste de processus ZOMBIE 
	  if (!found) {
	    cour = NULL;
	    queue_for_each(cour, &toKill_list, struct process, links) {
	      if (cour->pid==pid) {
		found = true;
		break;
	      }
	      found = false;
	    }
	  }

/* Cette fonction échoue et renvoie une valeur strictement négative s'il n'existe pas de processus avec ce pid
	  ou si ce n'est pas un fils du processus appelant. */
	  if (cour == NULL || !found) {
		  return -1;
	  }

	  while(cour->state == ACTIVABLE || cour->state == CHOSEN || cour->state == SLEEPING) {
		  sti();
	  }
	  cli();

	  // récupère sa valeur de retour dans *retvalp, à moins que retvalp soit nul.
	  if (retvalp != NULL)
	    *retvalp = cour->retval;
	  // Détruit le processus fils
	  queue_del(cour, links);
	  //queue_del(child, links);
	  // En cas de succès, elle retourne la valeur pid. 
	  return pid;
  } else {
    return -3; // 0: pid invalide
  }
  return -2;
}


int chprio(int pid, int newprio) {
  struct process* cour = NULL;
  int anc_prio;

	// Handle error case
  if (newprio < 1 || newprio > MAXPRIO) {
    return -1;
  }

  queue_add(chosen, &process_list, struct process, links, prio);  // WTF HERE ?

  queue_for_each(cour, &process_list, struct process, links) {
    if (cour->pid == pid) {
      anc_prio = cour->prio;
      cour->prio = newprio;
      break;
    }
  }

  if (cour != NULL && cour->state != ZOMBIE) {
      //si attente du proc ds une file, on le replace selon nvelle prio
      if (cour->state == SLEEPING) {
				queue_del(cour,links);
				queue_add(cour, &sleeping_list, struct process, links, prio);
      } else if (cour->state == ACTIVABLE){
				queue_del(cour,links);
				queue_add(cour, &process_list, struct process, links, prio);
      }
      queue_del(chosen,links);
      ordonnance();

      return anc_prio;
  }
  queue_del(chosen,links);
  return -1;
}

/**
 * Si pid invalide, retourne -1
 * sinon, retourne la priorité du processus pid. 
 */
int getprio(int pid) {
  if (pid < 0)
    return -1;

  if (chosen->pid == pid)
    return chosen->prio;
  
  struct process* cour;
  queue_for_each(cour, &process_list, struct process, links) {
    if (cour->pid == pid)
      return cour->prio;
  }

  return -1;
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
