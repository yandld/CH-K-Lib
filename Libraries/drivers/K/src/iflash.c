#include "iflash.h"
#include "common.h"

/* flash commands */
#define RD1BLK    0x00   
#define RD1SEC    0x01   
#define PGMCHK    0x02   
#define RDRSRC    0x03   
#define PGM4      0x06   
#define ERSBLK    0x08   
#define ERSSCR    0x09   
#define PGMSEC    0x0B   
#define RD1ALL    0x40   
#define RDONCE    0x41   
#define PGMONCE   0x43   
#define ERSALL    0x44   
#define VFYKEY    0x45   
#define PGMPART   0x80   
#define SETRAM    0x81   

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

#ifdef FTFL
#define FTFA    FTFL
#endif

static uint8_t CommandLaunch(void)
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


uint32_t FlashSignoff(void)
{
  /*Cache Invalidate all four ways
    Bank 0*/
  FMC->PFB0CR = FMC_PFB0CR_CINV_WAY(0xF);
  /*Invalidate (clear) specification buffer and page buffer
    Bank 0*/
 // FMC->PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;

  return FLASH_OK;
}

void FLASH_Init(void)
{
  FlashSignoff();
  /* wait until command complete */
  while(!(FTFA->FSTAT & CCIF));
  /* Clear status */
  FTFA->FSTAT = ACCERR | FPVIOL;

}


uint8_t FLASH_EraseSector(uint32_t sectorNo)
{
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)(sectorNo*(1<<11));

    /* set cmd */
	FTFA->FCCOB0 = ERSSCR; 

    /* set address */
	FTFA->FCCOB1 = dest.byte[2];
	FTFA->FCCOB2 = dest.byte[1];
	FTFA->FCCOB3 = dest.byte[0];
		
	if(FLASH_OK == CommandLaunch())
	{
		return FLASH_OK;
	}
	else
	{
		return FLASH_ERROR;
	}
}

uint8_t FLASH_ProgramWord(uint32_t sectorNo, uint8_t *buffer)
{
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)(sectorNo*(1<<11));

	FTFA->FCCOB0 = PGM4;

    /* set address */
    FTFA->FCCOB1 = dest.byte[2];
    FTFA->FCCOB2 = dest.byte[1];
    FTFA->FCCOB3 = dest.byte[0];

    FTFA->FCCOB4 = buffer[3];
    FTFA->FCCOB5 = buffer[2];
    FTFA->FCCOB6 = buffer[1];
    FTFA->FCCOB7 = buffer[0];
    dest.word += 4; buffer += 4;

    if(FLASH_OK != CommandLaunch())
    {
        return FLASH_ERROR;
    }
    return FLASH_OK;
}

uint8_t FLASH_WriteSector(uint32_t sectorNo, uint16_t count, uint8_t *buffer)
{
	uint16_t i;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)(sectorNo*(1<<11));

	FTFA->FCCOB0 = PGM4;

	for(i = 0; i < count; i += 4)
	{
        /* set address */
		FTFA->FCCOB1 = dest.byte[2];
		FTFA->FCCOB2 = dest.byte[1];
		FTFA->FCCOB3 = dest.byte[0];

		FTFA->FCCOB4 = buffer[3];
		FTFA->FCCOB5 = buffer[2];
		FTFA->FCCOB6 = buffer[1];
		FTFA->FCCOB7 = buffer[0];
		dest.word+=4; buffer+=4;

		if(FLASH_OK != CommandLaunch()) 
		return FLASH_ERROR;
    }
    return FLASH_OK;
}


