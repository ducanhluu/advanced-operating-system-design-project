#include "message_queue.h"
#include <stdlib.h>
//#include <stdio.h>
//#include "process.h"

#define NBQUEUE 5

int nb_queue = 0;
struct queue **queues = NULL;// /*malloc(/*NBQUEUE **/ 1/*sizeof(*queues)*/)*/;

/*int pcount(int fid, int *count){
  if (count != NULL) {

  }
  return -1;
  }*/

int pcreate(int count) {
  if (queues == NULL) {
    queues = malloc(NBQUEUE * sizeof(*queues));
  }
  if (nb_queue + 1 < NBQUEUE && count > 0) {
    nb_queue++;
    queues[nb_queue]->fid = nb_queue - 1;
    queues[nb_queue]->nb_message = 0;
    queues[nb_queue]->size_max = count;
    queues[nb_queue]->messages = malloc (count * sizeof(int));
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
  //free(queues[i]->messages);



  return 0;
}

/*
int preceive(int fid,int *message) {
   return -1;
}


int preset(int fid) {
   return -1;
}*/

int psend(int fid, int message) {
  if (fid >= nb_queue) {
    return -1;
  }
  if (queues[fid]->nb_message == 0 ){//}&& nb_p_bloques !=0) {
    queues[fid]->messages = &message;
    int a;
    //preceive(fid, &a);
  }
  if (queues[fid]->nb_message == queues[fid]->size_max) {

  }



// trouver la taille de la file



   return 0;
}
