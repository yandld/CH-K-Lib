#include <rtthread.h>
#include <stdint.h>
#include <rthw.h>
#include "components.h"

    
extern int Image$$RW_IRAM1$$ZI$$Limit;
extern int Image$$RW_IRAM1$$RW$$Limit;
extern int Image$$ER_IROM1$$RO$$Limit;

void rt_application_init(void);
void rt_heap_init(void);

void rtthread_startup(void)
{
	//rt_show_version();
	rt_system_timer_init();
    rt_system_scheduler_init();

    rt_device_init_all();
    rt_system_timer_thread_init();
    rt_thread_idle_init();
    rt_heap_init();
    rt_application_init();
    rt_system_scheduler_start();
}

/* redefine fputc */
int fputc(int ch,FILE *f)
{
    rt_kprintf("%c", ch);
    return ch;
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
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND - 1);
	rtthread_startup();
    
	return 0;
}

void SysTick_Handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}


void rt_system_comonent_init(void)
{

#ifdef RT_USING_MODULE
    rt_system_module_init();
#endif

#ifdef RT_USING_LWIP
	/* initialize lwip stack */
	/* register ethernetif device */
	eth_system_device_init();

	/* initialize lwip system */
	lwip_system_init();
	rt_kprintf("TCP/IP initialized!\n");
#endif

#ifdef RT_USING_DFS
	/* initialize the device file system */
	dfs_init();

#ifdef RT_USING_DFS_ELMFAT
	/* initialize the elm chan FatFS file system*/
	elm_init();
#endif

#if defined(RT_USING_DFS_NFS) && defined(RT_USING_LWIP)
	/* initialize NFSv3 client file system */
	nfs_init();
#endif

#ifdef RT_USING_DFS_YAFFS2
	dfs_yaffs2_init();
#endif

#ifdef RT_USING_DFS_UFFS
	dfs_uffs_init();
#endif

#ifdef RT_USING_DFS_JFFS2
	dfs_jffs2_init();
#endif

#ifdef RT_USING_DFS_ROMFS
	dfs_romfs_init();
#endif

#ifdef RT_USING_DFS_RAMFS
	dfs_ramfs_init();
#endif

#ifdef RT_USING_DFS_DEVFS
	devfs_init();
#endif
#endif /* end of RT_USING_DFS */

#ifdef RT_USING_NEWLIB
	libc_system_init(RT_CONSOLE_DEVICE_NAME);
#else
	/* the pthread system initialization will be initiallized in libc */
#ifdef RT_USING_PTHREADS 
	pthread_system_init();
#endif
#endif

#ifdef RT_USING_RTGUI
	rtgui_system_server_init();
#endif

#ifdef RT_USING_USB_HOST
	rt_usb_host_init();
#endif
}

void DelayMs(uint32_t ms)
{
    if(ms < (1000/RT_TICK_PER_SECOND)) rt_thread_delay(1);
    else rt_thread_delay(ms/(1000/RT_TICK_PER_SECOND));  
}
