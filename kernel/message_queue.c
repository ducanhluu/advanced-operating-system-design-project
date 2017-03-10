#include "message_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "../shared/stddef.h"
#include "process.h"

#define NBQUEUE 5

int nb_queue = 0;
struct queue **queues = NULL;

int pcount(int fid, int *count){
    int i = 0;
  while (i < nb_queue && queues[i]->fid != fid) {
    i++;
  }
  if (i == nb_queue) {
    return -1;
  }
  if (count != NULL) {
    if (queues[i]->nb_message == 0) {
      return - queues[i]->nb_p_bloques;
    }
    return queues[i]->nb_message + queues[i]->nb_p_bloques;
  }
  return 0;
 }

int pcreate(int count) {
  if (queues == NULL) {
    queues = mem_alloc(NBQUEUE * sizeof(*queues));
  }
  if (nb_queue + 1 < NBQUEUE && count > 0) {
    nb_queue++;
    queues[nb_queue]->fid = nb_queue - 1;
    queues[nb_queue]->nb_message = 0;
    queues[nb_queue]->size_max = count;
    queues[nb_queue]->messages = mem_alloc (count * sizeof(int));
    queues[nb_queue]->nb_p_bloques = 0;
    queues[nb_queue]->p_bloques = mem_alloc (PROCESS_TABLE_SIZE * sizeof(int));
    return nb_queue - 1;
  } else {
    return -1;
  }
}


int pdelete(int fid) {
  int i = 0;
  while (i < nb_queue && queues[i]->fid != fid) {
    i++;
  }
  if (i == nb_queue) {
    return -1;
  }
   //Les processus bloqués deviennent activables
  for (int j = 0; j < queues[i]->nb_p_bloques; j++) {
    unblock(queues[i]->p_bloques[j]);
  }
  nb_queue--;
  queues[i] = queues[nb_queue];
  mem_free(queues[i], 4 * sizeof(int) + PROCESS_TABLE_SIZE * sizeof(int) + queues[i]->size_max * sizeof(int));

  return 0;
}


int preceive(int fid,int *message) {
  int i = 0;
  while (i < nb_queue && queues[i]->fid != fid) {
    i++;
  }
  if (i == nb_queue) {
    return -1;
  }
  if (queues[i]->nb_message == 0) {
    //état bloqué sur file vide
    queues[i]->nb_p_bloques++;
    queues[i]->p_bloques[queues[i]->nb_p_bloques+1]=mon_pid();
    block_recv(mon_pid());
  }
  if (queues[i]->nb_message > 0) {
    *message = queues[i]->messages[0];
    queues[i]->nb_message--;
    for (int j=0; j<queues[i]->nb_message; j++) {
      queues[i]->messages[j] = queues[i]->messages[j+1];
    }
    if (queues[i]->nb_message == queues[i]->size_max - 1) {
      //la file était pleine
      if (queues[i]->nb_p_bloques > 0) {
	unblock(queues[i]->p_bloques[0]);
	queues[i]->nb_p_bloques--;
	for (int j=0; j<queues[i]->nb_p_bloques; j++) {
	  queues[i]->p_bloques[j] = queues[i]->p_bloques[j+1];
	}
      }
    }
  } else {
    //on a exécuté preset ou pdelete
    return -1;
  }
  return 0;

  //TODO le truc avec chprio :
  //Un processus bloqué sur file vide et dont la priorité est changée par chprio, est considéré comme le dernier processus (le plus jeune) de sa nouvelle priorité.
}



int preset(int fid) {
  int i = 0;
  while (i < nb_queue && queues[i]->fid != fid) {
    i++;
  }
  if (i == nb_queue) {
    return -1;
  }
  for (int j = 0; j < queues[i]->nb_p_bloques; j++) {
    unblock(queues[i]->p_bloques[j]);
  }
  queues[i]->nb_p_bloques = 0;
  mem_free(queues[i]->p_bloques, PROCESS_TABLE_SIZE * sizeof(int));
  queues[i]->nb_message = 0;
  mem_free(queues[i]->messages, queues[i]->size_max * sizeof(int));
  return 0;
}

int psend(int fid, int message) {
  if (fid >= nb_queue) {
    return -1;
  }
  if (queues[fid]->nb_message == 0 ){//}&& nb_p_bloques !=0) {
    queues[fid]->messages = &message;
    //int a;
    //preceive(fid, &a);
  }
  if (queues[fid]->nb_message == queues[fid]->size_max) {

  }



// trouver la taille de la file



   return 0;
}
