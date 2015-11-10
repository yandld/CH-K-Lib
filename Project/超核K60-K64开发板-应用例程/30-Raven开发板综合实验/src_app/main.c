#include <rtthread.h>
#include <stdint.h>
#include "pin.h"


extern uint32_t Image$$RW_IRAM1$$ZI$$Base[];
extern uint32_t Image$$RW_IRAM1$$ZI$$Limit[];
extern uint32_t Image$$RW_IRAM1$$Base[];
extern uint32_t Image$$RW_IRAM1$$Limit[];
extern uint32_t Load$$RW_IRAM1$$Base[];
extern uint32_t Load$$RW_IRAM1$$Limit[];

#define ZI_BASE         Image$$RW_IRAM1$$ZI$$Base
#define ZI_END          Image$$RW_IRAM1$$ZI$$Limit
#define RW_LOAD_BASE    Load$$RW_IRAM1$$Base
#define RW_LOAD_END     Load$$RW_IRAM1$$Limit
#define RW_BASE         Image$$RW_IRAM1$$Base
#define RW_END        Image$$RW_IRAM1$$Limit

void init_data_bss(void)
{
    uint32_t n;
    uint8_t *rw_ram;
    uint8_t *rw_rom;
    uint8_t *zi_ram;
    
    zi_ram  =(uint8_t*)&ZI_BASE;
    rw_ram = (uint8_t*)&RW_BASE;
    rw_rom = (uint8_t*)&RW_LOAD_BASE;
    rt_kprintf("ZI:0x%X\r\n", zi_ram);
    rt_kprintf("RWRAM:0x%X\r\n", rw_ram);
    rt_kprintf("RWROM:0x%X\r\n", rw_rom);
    n = (uint32_t)RW_END - (uint32_t)RW_BASE;
    while (n--)
        *rw_ram++ = *rw_rom++;
    
    n = (uint32_t)ZI_END - (uint32_t)ZI_BASE;
    while (n--)
        *zi_ram++ = 0;
}

#define PTA6    ((0<<8) + 6)



int Main(void* param) __attribute__((section("RESET")));
int Main(void* param)
{
    init_data_bss();
    rt_kprintf("app start up!\r\n");
    rt_pin_mode(PTA6, PIN_MODE_OUTPUT);
    rt_pin_write(PTA6, 0);
    
    ui_startup();
   // finsh_syscall_append("ui_startup", ui_startup);
    return 0;
}


