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



/* function return type */
#define FLASH_OK                    0x00
#define FLASH_OVERFLOW              0x01
#define FLASH_BUSY                  0x02
#define FLASH_ERROR                 0x04
#define FLASH_TIMEOUT               0x08
#define FLASH_NOT_ERASED            0x10
#define FLASH_CONTENTERR            0x11



/* disable interrupt before lunch command */
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)


#if defined(FTFL)
#define FTFA    FTFL
#elif defined(FTFE)
#define FTFA    FTFE
#endif

static uint8_t _CommandLaunch(void)
{
    /* Clear command result flags */
    FTFA->FSTAT = ACCERR | FPVIOL;

    /* Launch Command */
    FTFA->FSTAT = CCIF;

    /* wait command end */
    while(!(FTFA->FSTAT & CCIF));

    /*check for errors*/
    if(FTFA->FSTAT & (ACCERR | FPVIOL | MGSTAT0)) return FLASH_ERROR;

    /*No errors retur OK*/
    return FLASH_OK;

}

void FLASH_Init(void)
{
  /* Clear status */
  FTFA->FSTAT = ACCERR | FPVIOL;
}


uint8_t FLASH_EraseSector(uint32_t addr)
{
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

    /* set cmd */
	FTFA->FCCOB0 = ERSSCR; 
	FTFA->FCCOB1 = dest.byte[2];
	FTFA->FCCOB2 = dest.byte[1];
	FTFA->FCCOB3 = dest.byte[0];
		
	if(FLASH_OK == _CommandLaunch())
	{
		return FLASH_OK;
	}
	else
	{
		return FLASH_ERROR;
	}
}

#if 0 
uint8_t FLASH_ProgramWord(uint32_t sectorNo, uint8_t *buf)
{
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)(sectorNo*(DEFAULT_SECTOR_SIZE));

	FTFA->FCCOB0 = PGM4;

    /* set address */
    FTFA->FCCOB1 = dest.byte[2];
    FTFA->FCCOB2 = dest.byte[1];
    FTFA->FCCOB3 = dest.byte[0];

    FTFA->FCCOB4 = buf[3];
    FTFA->FCCOB5 = buf[2];
    FTFA->FCCOB6 = buf[1];
    FTFA->FCCOB7 = buf[0];
    dest.word += 4; buf += 4;

    if(FLASH_OK != _CommandLaunch())
    {
        return FLASH_ERROR;
    }
    return FLASH_OK;
}
#endif

uint8_t FLASH_WriteSector(uint32_t addr, uint8_t *buf, uint32_t len)
{
	uint16_t i;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;

	FTFA->FCCOB0 = PGM4;

	for(i=0; i<len; i+=4)
	{
        /* set address */
		FTFA->FCCOB1 = dest.byte[2];
		FTFA->FCCOB2 = dest.byte[1];
		FTFA->FCCOB3 = dest.byte[0];

		FTFA->FCCOB4 = buf[3];
		FTFA->FCCOB5 = buf[2];
		FTFA->FCCOB6 = buf[1];
		FTFA->FCCOB7 = buf[0];
        
        FTFA->FCCOB8 = buf[7];
        FTFA->FCCOB9 = buf[6];
        FTFA->FCCOBA = buf[5];
        FTFA->FCCOBB = buf[4];
        FTFA->FCCOB4 = buf[3];
        FTFA->FCCOB5 = buf[2];
        FTFA->FCCOB6 = buf[1];
        FTFA->FCCOB7 = buf[0];
        
		dest.word += 4; buf += 4;

		if(FLASH_OK != _CommandLaunch()) 
        {
            return FLASH_ERROR;
        }
    }
    return FLASH_OK;
}


