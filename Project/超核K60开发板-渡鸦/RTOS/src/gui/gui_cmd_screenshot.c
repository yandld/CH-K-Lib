/*
 * File      : writespeed.c
 * This file is part of RT-TestCase in RT-Thread RTOS
 * COPYRIGHT (C) 2010, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2010-02-10     Bernard      first version
 */
#include <rtthread.h>
#include <dfs_posix.h>
#include "GUI.H"

static void _WriteByte2File(U8 Data, void *p)
{
    write(*(int*)p, &Data, 1);
}


void screen_shot(const char* filename)
{
    int fd, index, length;
    char *buff_ptr;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0);
    if (fd < 0)
    {
        rt_kprintf("open file:%s failed\n", filename);
        return;
    }
    GUI_BMP_Serialize(_WriteByte2File, &fd);
    rt_kprintf("scrren cut completed @ file:%s\r\n", filename);
    close(fd);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(screen_shot, screen shot);
#endif
