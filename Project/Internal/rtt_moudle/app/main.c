#include <rtthread.h>
#include <stdint.h>
#include "rtt_api.h"

api_t *a;

int main(void) __attribute__((section(".ARM.__at_0x40100")));
int main(void)
{
    a = (api_t*)(0x30000);
    a->rtthread->rt_kprintf("HelloWorld\r\n");
    while(1)
    {
        a->rtthread->rt_kprintf("Loop\r\n");
        a->rtthread->rt_thread_delay(100);
    }
}


