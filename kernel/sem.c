#include "sem.h"
#include "../shared/stddef.h"
#include "stdio.h"
#include "process.h"

int nb_sem = 0;
struct sem *sems = NULL;

struct sem *sem_courant(int sem) {
    struct sem *cou = sems;
    while (cou != NULL) {
        if (cou->sid == sem) { 
            break;
        }
        cou = cou->next;
    }

    return cou;
}

void add_proc_bloque(struct sem *cou, int pid) {
    struct procs_bloques *c = cou->p_bloques;
    if (c == NULL) {
        c = (struct procs_bloques *)mem_alloc(sizeof(struct procs_bloques));
        c->pid = pid;
        c->next = NULL;
    } else {
        while (c != NULL) {
            c = c->next;
        }
        c->pid = pid;
        c->next = NULL;
    }
    
}

int get_premier_proc_bloque(struct sem *cou) {
    if (cou->p_bloques == NULL) {
        return -1; // Aucun processus bloqué
    }
    
    struct procs_bloques *c = cou->p_bloques;
    cou->p_bloques = cou->p_bloques->next;
    int ret = c->pid;
    mem_free(c, sizeof(struct procs_bloques)); // free mémoire
    return ret;
}

int scount(int sem) {
    if (sems == NULL || sem < 0) {
        return -1;
    }

    struct sem *cou = sem_courant(sem);
     
    return cou->val;
}

int screate(short count) {
    nb_sem++;
    if (nb_sem > NBSEMS || count < 0) {
        return -1;
    }

    if (sems == NULL) {
        sems = (struct sem *)mem_alloc(sizeof(struct sem));
        sems->sid = nb_sem - 1;
        sems->val = count;
        sems->next = NULL;
        sems->p_bloques = NULL;
        return sems->sid;
    } else {
        struct sem *cou = sems;

        while (cou->next != NULL) {
            cou = cou->next;
        }
	
	cou->next = (struct sem *)mem_alloc(sizeof(struct sem));
	cou = cou->next;
	cou->sid = nb_sem - 1;
        cou->val = count;
        cou->next = NULL;
        cou->p_bloques = NULL;
        return cou->sid;
    }
}

int sdelete(int sem) {
    if (sem < 0 || sem >= NBSEMS) {
        return -1;
    }
    
    struct sem *cou = sem_courant(sem);

    // Passe tous les processus bloqués sur sémaphore sem à l'état activable
   
    int pid = get_premier_proc_bloque(cou);
    
    while (pid != -1) {
        passe_activable(pid);
        pid = get_premier_proc_bloque(cou);
    } 
    
    // rend la valeur identification de sémaphore invalide
    cou->sid = -1;
    return 0;
}

int signal(int sem) {
    if (sem < 0 || sem >= NBSEMS) {
        return -1; // Valeur de sem est invalide
    }

    struct sem *cou = sem_courant(sem);
        
    // Vérification de dépassement de capacité
    if (cou->val + 1 > 32767) {
        return -2; // Dépassement de capacité
    }
    
    cou->val++;
    if (cou->val <= 0) {
        // Premier processus bloque passe dans l'état activable ou actif
        int pid = get_premier_proc_bloque(cou);
        if (pid != -1) {
            passe_activable(pid);
        }
    }
    ordonnance();
    return 0;
}

int signaln(int sem, short count) {
    if (sem < 0 || sem >= NBSEMS || count < 0) {
        return -1; // Valeur de sem est invalide
    }

    struct sem *cou = sem_courant(sem);
    
    // Vérification de dépassement de capacité
    
    if (cou->val + count > 32767) { //Passe 32767
        return -2;
    }
    

    while (count > 0) {  
        cou->val++;
        if (cou->val <= 0) {
            int pid = get_premier_proc_bloque(cou);
            if (pid != -1) {
                passe_activable(pid);
            }
        }
        count--;
    }
    ordonnance();
    return 0;
}

int sreset(int sem, short count) {
    if (sem < 0 || sem >= NBSEMS || count < 0) {
        return -1; // Valeur invalide
    }
    struct sem *cou = sem_courant(sem);

    // Passe tous les processus bloqués sur sémaphore sem à l'état activable
   
    int pid = get_premier_proc_bloque(cou);
    
    while (pid != -1) {
        passe_activable(pid);
        pid = get_premier_proc_bloque(cou);
    } 

    // Associe la valeur count à ce sémaphore
    cou->val = count;
    return 0;
}

int try_wait(int sem) {
    if (sem < 0 || sem >= NBSEMS) {
        return -1; // Valeur invalide
    }
    
    struct sem *cou = sem_courant(sem);
    
    if (cou->val - 1 <= 0) {
        return -3; // Compteur est négatif ou nul
    }
    
    if (cou->val - 1 < -32768) {
        return -2;  // Dépassement de capacité
    } else if (cou->val > 0) {
        cou->val--;
    }
    
    return 0;
}

int wait(int sem) {
    if (sem < 0 || sem >= NBSEMS) {
        return -1; // Valeur de sem est invalide
    }

    struct sem *cou = sem_courant(sem);
    if (cou->val - 1 < -32768) {
        return -2;  // Dépassement de capacité
    } else {
        // Décrémente de 1 la valeur du sémaphore identifié par sem
        cou->val--;
    }
    int estBloque = 0;
    if (cou->val < 0) {
        // Processus passe de l'état actif à l'état bloqué sur le sémaphore sem
        add_proc_bloque(cou, getpid());
        bloque_sur_semaphore();
        estBloque = 1;
    }
    
    if (cou->sid == -1) { 
        return -3; // Reveil est consécutif à sdelete
    }

    if (estBloque == 1) {
        return -4; // Reveil est consécutif à sreset
    }
    return 0;
}
