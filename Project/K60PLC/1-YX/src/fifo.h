#ifndef __CH_FIFO_H__
#define __CH_FIFO_H__

#include "fifo.h"
#include <stdint.h>

#define FLAGS_OVERRUN 0x0001  

/*  
        buf- ?????  
        size- ??  
        free- ????  
        putP- ?????????  
        getP- ?????????  
*/  
struct FIFO
{  
    uint16_t *buf;  
    int putP,getP,size,free,flags;  
};  
  
void fifo_init(struct FIFO *fifo, int size, uint16_t *buf);  
int fifo_put(struct FIFO *fifo, uint16_t data);  
int fifo_get(struct FIFO *fifo);  
int fifo_status(struct FIFO *fifo);  
int fifo_free(struct FIFO *fifo);  



#endif


