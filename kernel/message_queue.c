#include "message_queue.h"
#include <stdlib.h>
//#include <stdio.h>
//#include "process.h"

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

int pcreate(int count){
  if (queues == NULL) {
    queues = malloc(NBQUEUE * sizeof(*queues));
  }
  if (nb_queue + 1 < NBQUEUE && count > 0) {
    nb_queue++;
    queues[nb_queue]->fid = nb_queue - 1;
    queues[nb_queue]->nb_message = 0;
    queues[nb_queue]->size_max = count;
    queues[nb_queue]->messages = malloc (count * sizeof(int));
    queues[nb_queue]->nb_p_bloques = 0;
    queues[nb_queue]->p_bloques = malloc (PROCESS_TABLE_SIZE * sizeof(int));
    return nb_queue - 1;
  } else {
    return -1;
  }
}


int pdelete(int fid){
  int i = 0;
  while (i < nb_queue && queues[i]->fid != fid) {
    i++;
  }
  if (i == nb_queue) {
    return -1;
  }
  for (int j = 0; j < queues[i]->nb_p_bloques; j++) {

  }
  queues[i]->nb_p_bloques = 0;
  //free(queues[i]->p_bloques);
  queues[i]->nb_message = 0;
  queues[i]->size_max = 0;
  //free(queues[i]->messages);




  return 0;
}

/*
int preceive(int fid,int *message) {
   return -1;
}
int preset(int fid){
   return -1;
}
int psend(int fid, int message) {
   return -1;
}
*/
