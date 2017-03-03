#include "sem.h"
#include "../shared/stddef.h"
int nb_sem = 0;
struct sem *sems = NULL;


int scount(int sem) {
    if (sems == NULL || sem < 0) {
        return -1;
    }
    struct sem *cou = sems;
    
    while (cou != NULL) {
        if (cou->sid == sem) { 
            return cou->val;
        }
        cou = cou->next;
    }

    return -1;
}

int screate(short count) {
    nb_sem++;
    if (nb_sem > NBSEM || count < 0) {
        return -1;
    }

    if (sems == NULL) {
        sems = mem_alloc(sizeof(struct sem));
        sems->sid = nb_sem - 1;
        sems->val = count;
        sems->next = NULL;
        return sems->sid;
    } else {
        struct sem *cou = sems;

        while (cou != NULL) {
            cou = cou->next;
        }
        cou->sid = nb_sem - 1;
        cou->val = count;
        cou->next = NULL;
        return cou->sid;
    }
}

int sdelete(int sem) {
    return sem;
}

int signal(int sem) {
    if (sem < 0 || sem >= NBSEM) {
        return -1;
    }

    struct sem *cou = sems;
    
    while (cou != NULL) {
        if (cou->sid == sem) { 
            cou->val++;
            //TODO
            break;
        }
        cou = cou->next;
    }
    return 0;

    // return -2; TODO
}

int signaln(int sem, short count) {
    int ret;
    while (count > 0) {
        ret = signal(sem);
        if (ret != 0) {
            //TODO
            break;
        }

        count--;
    }

    return ret;
}

int sreset(int sem, short count) {
    if (sem < 0 || sem >= NBSEM || count < 0) {
        return -1;
    }

    //TODO
    struct sem *cou = sems;
    
    while (cou != NULL) {
        if (cou->sid == sem) { 
            cou->val = count;
            break;
        }
        cou = cou->next;
    }
    return 0;
}

int try_wait(int sem) {
    return sem;
}

int wait(int sem) {
     if (sem < 0 || sem >= NBSEM) {
        return -1;
    }

    struct sem *cou = sems;
    
    while (cou != NULL) {
        if (cou->sid == sem) { 
            cou->val--;
            // TODO
            break;
        }
        cou = cou->next;
    }
    return 0;
}
