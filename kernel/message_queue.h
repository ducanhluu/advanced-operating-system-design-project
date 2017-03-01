#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_
#if 0
#include "../shared/malloc.c"
#else
void *mem_alloc(unsigned long length);
void mem_free(void *zone, unsigned long length);
#endif

#include "process.h"

/*int pcount(int fid, int *count);*/
int pcreate(int count);
/*int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);*/
int psend(int fid, int message);

struct queue {
  int fid;
  int nb_message;
  int size_max;
  int *messages;
  int nb_p_bloques;
  int *p_bloques; //processus bloques sur la file (pids stockés)
};

#endif
