#include <rtthread.h>z
#include <stdint.h>
#include <rthw.h>
#include "components.h"

const static unsigned char _dummy_dummy_txt[] =
{
    0x74,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x66,0x69,0x6c,0x65,0x21,0x0d,0x0a,
};

const static struct romfs_dirent _dummy[] =
{
    {ROMFS_DIRENT_FILE, "dummy.txt", _dummy_dummy_txt, sizeof(_dummy_dummy_txt)},
};

const static unsigned char _dummy_txt[] =
{
    0x74,0x68,0x69,0x73,0x20,0x69,0x73,0x20,0x61,0x20,0x66,0x69,0x6c,0x65,0x21,0x0d,0x0a,
};

const struct romfs_dirent _root_dirent[] =
{
    {ROMFS_DIRENT_DIR, "dummy", (rt_uint8_t *)_dummy, sizeof(_dummy)/sizeof(_dummy)},
    {ROMFS_DIRENT_DIR, "SD", (rt_uint8_t *)RT_NULL, RT_NULL},
    {ROMFS_DIRENT_DIR, "SF", (rt_uint8_t *)RT_NULL, RT_NULL},
    {ROMFS_DIRENT_DIR, "NFS", (rt_uint8_t *)RT_NULL, RT_NULL},
    {ROMFS_DIRENT_FILE, "dummy.txt", _dummy_txt, sizeof(_dummy_txt)},
};



#define RTT_INITAL_HEAP_SIZE   (1024*6)
static uint8_t InitalMemPoll[RTT_INITAL_HEAP_SIZE];
    
extern int Image$$RW_IRAM1$$ZI$$Limit;
extern int Image$$RW_IRAM1$$RW$$Limit;
extern int Image$$ER_IROM1$$RO$$Limit;

void rt_application_init(void);

void rtthread_startup(void)
{
	//rt_show_version();
	rt_system_timer_init();
    rt_system_scheduler_init();

    rt_device_init_all();
    rt_system_timer_thread_init();
    rt_thread_idle_init();
    rt_application_init();
    rt_system_scheduler_start();
}

/** \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
  volatile uint32_t CTRL;                    /*!< Offset: 0x000 (R/W)  SysTick Control and Status Register */
  volatile uint32_t LOAD;                    /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register       */
  volatile uint32_t VAL;                     /*!< Offset: 0x008 (R/W)  SysTick Current Value Register      */
  volatile uint32_t CALIB;                   /*!< Offset: 0x00C (R/ )  SysTick Calibration Register        */
} SysTick_Type;

#define SysTick_CTRL_CLKSOURCE_Pos          2                                             /*!< SysTick CTRL: CLKSOURCE Position */
#define SysTick_CTRL_CLKSOURCE_Msk         (1UL << SysTick_CTRL_CLKSOURCE_Pos)            /*!< SysTick CTRL: CLKSOURCE Mask */

#define SysTick_CTRL_TICKINT_Pos            1                                             /*!< SysTick CTRL: TICKINT Position */
#define SysTick_CTRL_TICKINT_Msk           (1UL << SysTick_CTRL_TICKINT_Pos)              /*!< SysTick CTRL: TICKINT Mask */

#define SysTick_CTRL_ENABLE_Pos             0                                             /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL << SysTick_CTRL_ENABLE_Pos)               /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0                                             /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL << SysTick_LOAD_RELOAD_Pos)        /*!< SysTick LOAD: RELOAD Mask */

#define SysTick             ((SysTick_Type   *)(0xE000E000UL +  0x0010UL))   /*!< SysTick configuration struct       */

uint32_t SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1) > SysTick_LOAD_RELOAD_Msk)  return (1);      /* Reload value impossible */

    SysTick->LOAD  = ticks - 1;                                  /* set reload register */
    SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  return (0); 
}

extern uint32_t SystemCoreClock;

__weak int main(void)
{
	rt_hw_interrupt_disable();
    
    /* set system heap */
    uint32_t begin_addr = ((uint32_t)InitalMemPoll);
    uint32_t end_addr = begin_addr + RTT_INITAL_HEAP_SIZE;
    
    rt_system_heap_init((void*)begin_addr, (void*)end_addr);
    
    /* init systick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND - 1);
    
	rtthread_startup();
    
	return 0;
}





