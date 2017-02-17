#ifndef MESSAGE_QUEUE_H_
#define MESSAGE_QUEUE_H_
#if 0
#include "../shared/malloc.c"
#else
extern void *malloc(int size);
#define NULL 0
#endif

/*int pcount(int fid, int *count);*/
int pcreate(int count);
/*int pdelete(int fid);
int preceive(int fid,int *message);
int preset(int fid);
int psend(int fid, int message);*/

struct queue {
  int fid;
  int nb_message;
  int size_max;
  int *messages;
  int *p_bloques; //processus bloques sur la file
};

#endif
