/*
 * File      : romfs.c
 * This file is part of Device File System in RT-Thread RTOS
 * COPYRIGHT (C) 2004-2011, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <dfs_romfs.h>

const static unsigned char _dummy_txt[] = "romfs, read only!";


const struct romfs_dirent _root_dirent[] =
{
    {ROMFS_DIRENT_DIR, "SD", RT_NULL, RT_NULL},
    {ROMFS_DIRENT_DIR, "SF", RT_NULL, RT_NULL},
    {ROMFS_DIRENT_DIR, "NFS", RT_NULL, RT_NULL},
    {ROMFS_DIRENT_DIR, "MNT", RT_NULL, RT_NULL},
    {ROMFS_DIRENT_FILE, "dummy.txt", _dummy_txt, sizeof(_dummy_txt)},
};

const struct romfs_dirent romfs_root =
{
    ROMFS_DIRENT_DIR, "/", (rt_uint8_t *)_root_dirent, sizeof(_root_dirent)/sizeof(_root_dirent[0])
};

