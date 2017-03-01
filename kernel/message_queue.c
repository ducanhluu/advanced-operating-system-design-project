#include "message_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "../shared/stddef.h"
#include "process.h"

#define NBQUEUE 5

int nb_queue = 0;
struct queue **queues = NULL;// /*malloc(/*NBQUEUE **/ 1/*sizeof(*queues)*/)*/;

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
    maj_sleeping(queues[i]->p_bloques[j]);
  }
  queues[i]->nb_p_bloques = 0;
  mem_free(queues[i]->p_bloques, PROCESS_TABLE_SIZE * sizeof(int));
  queues[i]->nb_message = 0;
  queues[i]->size_max = 0;
  mem_free(queues[i]->messages, queues[i]->size_max * sizeof(int));
  return 0;
}


int preceive(int fid,int *message) {
  fid++;
  *message=0;
   return -1;
}


/*
int preset(int fid) {
   return -1;
}*/

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
