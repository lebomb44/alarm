#include "global.h"
#include "fifo.h"

void fifo_init(FIFO_T* fifo)
{
  /* Reset index */
  fifo->put=0;
  fifo->get=0;
}

void fifo_put(FIFO_T* fifo, u08 data)
{
  /* Check if fifo is not full */
  if(((fifo->put)+1) % FIFO_SIZE != (fifo->get))
  {
    /* Commit data into the buffer */
    fifo->buff[fifo->put] = data;
    /* Prepare index for next data */
    fifo->put++;
    /* Return to begin of buffer as it is a ring buffer */
    fifo->put = (fifo->put) % FIFO_SIZE;
  }
}

u16 fifo_get(FIFO_T* fifo)
{
  u16 data=0;
  /* Check if fifo is not empty */
  if(fifo->get != fifo->put)
  {
    /* Checkout data */
    data = fifo->buff[fifo->get];
    /* Prepare index for next data */
    fifo->get++;
    /* Return to begin of buffer as it is a ring buffer */
    fifo->get = (fifo->get) % FIFO_SIZE;
  }
  /* if buffer is empty */
  else
  {
    /* Fullfill data to 16bits to make the difference to usable data on 8bits */
    data = 0xFFFF;
  }
  return data;
}

void fifo_flush(FIFO_T* fifo)
{
  while(fifo_get(fifo) != 0xFFFF);
}
