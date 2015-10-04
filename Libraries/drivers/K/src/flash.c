/**
  * @file    flash.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @date    2015.10.05 FreeXc 完善了flash模块中API函数的注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  */
  
#include "flash.h"
#include "common.h"


/* flash commands */
#define RD1BLK    0x00  /* read 1 block */
#define RD1SEC    0x01  /* read 1 section */
#define PGMCHK    0x02  /* program check */
#define RDRSRC    0x03  /* read resource */
#define PGM4      0x06  /* program phase program 4 byte */
#define PGM8      0x07  /* program phase program 8 byte */
#define ERSBLK    0x08  /* erase flash block */
#define ERSSCR    0x09  /* erase flash sector */
#define PGMSEC    0x0B  /* program section */
#define RD1ALL    0x40  /* read 1s all block */
#define RDONCE    0x41  /* read once */
#define PGMONCE   0x43  /* program once */
#define ERSALL    0x44  /* erase all blocks */
#define VFYKEY    0x45  /* verift backdoor key */
#define PGMPART   0x80  /* program paritition */
#define SETRAM    0x81  /* set flexram function */
#define NORMAL_LEVEL 0x0




/* disable interrupt before launch command */
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)

#if defined(FTFL)
#define FTF    FTFL
#define SECTOR_SIZE     (2048)
#define PROGRAM_CMD      PGM4
#elif defined(FTFE)
#define FTF    FTFE
#define SECTOR_SIZE     (4096)
#define PROGRAM_CMD      PGM8
#elif defined(FTFA)
#define SECTOR_SIZE     (1024)
#define PROGRAM_CMD      PGM4
#define FTF    FTFA
#endif

/**
 * \brief luanch command
 * \note Internal function, user needn't use
 * \retval 0x00 launch successfully
 * \retval 0x04 launch failure
 */
static uint8_t _CommandLaunch(void)
{
    /* Clear command result flags */
    FTF->FSTAT = ACCERR | FPVIOL;

    /* Launch Command */
    FTF->FSTAT = CCIF;

    /* wait command end */
    while(!(FTF->FSTAT & CCIF));

    /*check for errors*/
    if(FTF->FSTAT & (ACCERR | FPVIOL | MGSTAT0)) return FLASH_ERROR;

    /*No errors retur OK*/
    return FLASH_OK;

}

/**
 * \brief get the flash sector size
 * \return sector size
 */
uint32_t FLASH_GetSectorSize(void)
{
    return SECTOR_SIZE;
}

/**
 * \brief Flash Initialization
 * \note clear the flags : Flash Access Error Flag &Flash Protection Violation Flag
 * \retval None
 */
void FLASH_Init(void)
{
    /* Clear status */
    FTF->FSTAT = ACCERR | FPVIOL;
}

/**
 * \brief erase Flash sector
 * \param[in] addr 待擦除块的地址
 * \retval None
 */
uint8_t FLASH_EraseSector(uint32_t addr)
{
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

    /* set cmd */
	FTF->FCCOB0 = ERSSCR; 
	FTF->FCCOB1 = dest.byte[2];
	FTF->FCCOB2 = dest.byte[1];
	FTF->FCCOB3 = dest.byte[0];
		
	if(FLASH_OK == _CommandLaunch())
	{
		return FLASH_OK;
	}
	else
	{
		return FLASH_ERROR;
	}
}
/**
 * \brief Flash块写入操作
 * \param[in] addr 写入的目的地址
 * \param[in] buf 字符串指针
 * \param[in] len 写入大小
 * \retval 0x00 FLASH_OK
 * \retval 0x04 FLASH_ERROR
 */
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
	uint16_t i;
    uint16_t step;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

	FTF->FCCOB0 = PROGRAM_CMD;
    
    switch(PROGRAM_CMD)
    {
        case PGM4:
            step = 4;
            break;
        case PGM8:
            step = 8;
            break;
        default:
            LIB_TRACE("FLASH: no program cmd found!\r\n");
            step = 4;
            break;
    }
    
	for(i=0; i<len; i+=step)
	{
        /* set address */
		FTF->FCCOB1 = dest.byte[2];
		FTF->FCCOB2 = dest.byte[1];
		FTF->FCCOB3 = dest.byte[0];

		FTF->FCCOB4 = buf[3];
		FTF->FCCOB5 = buf[2];
		FTF->FCCOB6 = buf[1];
		FTF->FCCOB7 = buf[0];
        
        if(step == 8)
        {
            FTF->FCCOB8 = buf[7];
            FTF->FCCOB9 = buf[6];
            FTF->FCCOBA = buf[5];
            FTF->FCCOBB = buf[4];
        }

		dest.word += step; buf += step;

		if(FLASH_OK != _CommandLaunch()) 
        {
            return FLASH_ERROR;
        }
    }
    return FLASH_OK;
}


