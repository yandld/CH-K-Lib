#ifndef _USRPICTURE_H
#define _USRPICTURE_H

#include "stdint.h"

/**
 * \struct Picture 结构体
 * \brief 用于存放图片的地址和大小
 */
typedef struct{
	const unsigned char *picAddr;
	uint32_t picSize;
}Picture;

/* C文件代表的图片信息 */
extern Picture jpgHankeku;

#endif
