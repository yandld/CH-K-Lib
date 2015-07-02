#include <rtthread.h>
#include <stdint.h>
#include "pin.h"


extern uint32_t Image$$RW_IRAM2$$ZI$$Base[];
extern uint32_t Image$$RW_IRAM2$$ZI$$Limit[];
extern uint32_t Image$$RW_IRAM2$$Base[];
extern uint32_t Image$$RW_IRAM2$$Limit[];
extern uint32_t Load$$RW_IRAM2$$Base[];
extern uint32_t Load$$RW_IRAM2$$Limit[];

#define ZI_BASE         Image$$RW_IRAM2$$ZI$$Base
#define ZI_END          Image$$RW_IRAM2$$ZI$$Limit
#define RW_LOAD_BASE    Load$$RW_IRAM2$$Base
#define RW_LOAD_END     Load$$RW_IRAM2$$Limit
#define RW_BASE         Image$$RW_IRAM2$$Base
#define RW_END        Image$$RW_IRAM2$$Limit

void init_data_bss(void)
{
    uint32_t n;
    uint8_t *rw_ram;
    uint8_t *rw_rom;
    uint8_t *zi_ram;
    
    zi_ram  =(uint8_t*)&ZI_BASE;
    rw_ram = (uint8_t*)&RW_BASE;
    rw_rom = (uint8_t*)&RW_LOAD_BASE;
    
    n = (uint32_t)RW_END - (uint32_t)RW_BASE;
    while (n--)
        *rw_ram++ = *rw_rom++;
    
    n = (uint32_t)ZI_END - (uint32_t)ZI_BASE;
    while (n--)
        *zi_ram++ = 0;
}

#define PTA6    ((0<<8) + 6)


int main2(void* param) __attribute__((section(".ARM.__at_0x70400")));
int main2(void* param)
{
    int ret;
    init_data_bss();
    
    rt_device_t gpio;
    gpio = rt_device_find("gpio");
    rt_pin_mode(PTA6, PIN_MODE_OUTPUT);
    uint32_t i;
    rt_pin_write(PTA6, 0);
    
    ui_startup(0, 0);
//    finsh_syscall_append("ui_startup", ui_startup);
}


