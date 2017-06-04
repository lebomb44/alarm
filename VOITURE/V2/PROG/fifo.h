#ifndef _FIFO_
#define _FIFO_

#include "global.h"

#define FIFO_SIZE 100

typedef struct _FIFO_T {
  u08 put;
  u08 get;
  u08 buff[FIFO_SIZE];
} FIFO_T;

void fifo_init(FIFO_T* fifo);
void fifo_put(FIFO_T* fifo, u08 data);
u16 fifo_get(FIFO_T* fifo);
void fifo_flush(FIFO_T* fifo);

#endif

