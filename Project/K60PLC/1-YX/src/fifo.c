#include "fifo.h"
#include <stdio.h>

void fifo_init(struct FIFO *fifo,int size, uint16_t *buf)  
/*???*/  
{  
        fifo->buf = buf;  
        fifo->flags = 0;            
        fifo->free = size;  
        fifo->size = size;  
        fifo->putP = 0;                     
        fifo->getP = 0;                     
  
         return;  
} 

int fifo_put(struct FIFO *fifo, uint16_t data)  
{  
    if(fifo->free==0)
    {  
        fifo->flags |= FLAGS_OVERRUN;  
        return -1;  
    }  
    fifo->buf[fifo->putP] = data;  
    fifo->putP++;  
    //Ñ­»·¶ÓÁÐ»º³å
    if(fifo->putP == fifo->size)
    {  
        fifo->putP = 0;  
    }  
    fifo->free--;  
  
    return 0;  
}  

int fifo_get(struct FIFO *fifo)  
/*?FIFO ??????? */  
{  
    int data;  
    if(fifo->free == fifo->size)
    {  
        return -1;  
    }  
    data = fifo->getP;  
    fifo->getP++;  
    if(fifo->getP == fifo->size)
    {
        fifo->getP = 0;  
    }  
    fifo->free++;  
          
    return data;  
}  


int fifo_status(struct FIFO *fifo)   
{  
    return fifo->size-fifo->free;  
}

int fifo_free(struct FIFO *fifo)  
{  
    return fifo->free;  
}

int fifo_test(void)
{
    struct FIFO fifo;
    uint16_t buf[64];
    uint16_t test_buf[] = {1,2,3,4,5,6,7,8,9,10};
    fifo_init(&fifo, 16, buf);
    fifo_put(&fifo, 1);
    fifo_put(&fifo, 2);
    fifo_put(&fifo, 3);
    fifo_put(&fifo, 4);
    fifo_put(&fifo, 5);
    printf("get once:%d\r\n", buf[fifo_get(&fifo)]);
    printf("get once:%d\r\n", buf[fifo_get(&fifo)]);
    printf("get once:%d\r\n", buf[fifo_get(&fifo)]);
    printf("free:%d\r\n", fifo_free(&fifo));
    
    return 0;
}



