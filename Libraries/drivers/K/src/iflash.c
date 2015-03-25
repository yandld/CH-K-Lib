#include "iflash.h"
#include "common.h"

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

#define FLASH_OK                    0x00
#define FLASH_OVERFLOW              0x01
#define FLASH_BUSY                  0x02
#define FLASH_ERROR                 0x04
#define FLASH_TIMEOUT               0x08
#define FLASH_NOT_ERASED            0x10
#define FLASH_CONTENTERR            0x11

//#if 0
//struct FlashSectors  {
//  unsigned long   szSector;    // Sector Size in Bytes
//  unsigned long AddrSector;    // Address of Sector
//};

//struct FlashDevice  {
//   unsigned short     Vers;    // Version Number and Architecture
//   char       DevName[128];    // Device Name and Description
//   unsigned short  DevType;    // Device Type: ONCHIP, EXT8BIT, EXT16BIT, ...
//   unsigned long    DevAdr;    // Default Device Start Address
//   unsigned long     szDev;    // Total Size of Device
//   unsigned long    szPage;    // Programming Page Size
//   unsigned long       Res;    // Reserved for future Extension
//   unsigned char  valEmpty;    // Content of Erased Memory

//   unsigned long    toProg;    // Time Out of Program Page Function
//   unsigned long   toErase;    // Time Out of Erase Sector Function

//   struct FlashSectors sectors[SECTOR_NUM];
//};


//#define FLASH_DRV_VERS (0x0100+1)   // Driver Version, do not modify!
//#define SECTOR_END 0xFFFFFFFF, 0xFFFFFFFF
//#define ONCHIP     1           // On-chip Flash Memory

//struct FlashDevice const FlashDevice  =  {
//   FLASH_DRV_VERS,             // Driver Version, do not modify!
//   "MKXX 256kB Prog Flash",    // Device Name 
//   ONCHIP,                     // Device Type
//   0x00000000,                 // Device Start Address
//   0x00040000,                 // Device Size (256kB)
//   512,                        // Programming Page Size
//   0,                          // Reserved, must be 0
//   0xFF,                       // Initial Content of Erased Memory
//   1000,                       // Program Page Timeout 1000 mSec
//   3000,                       // Erase Sector Timeout 3000 mSec

//// Specify Size and Address of Sectors
//   0x000800, 0x000000,         // Sector Size  2kB (128 Sectors)
//   SECTOR_END
//};
//#endif

/* disable interrupt before lunch command */
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)

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

void FLASH_Init(void)
{
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

uint8_t FLASH_ProgramWord(uint32_t sectorNo, const uint8_t *buffer)
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
    dest.word+=4; buffer+=4;

    if(FLASH_OK != CommandLaunch()) 
    return FLASH_ERROR;
    return FLASH_OK;
}

uint8_t FLASH_WriteSector(uint32_t sectorNo, uint16_t count, uint8_t const *buffer)
{
	uint16_t i;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)(sectorNo*(1<<11));

	FTFA->FCCOB0 = PGM4;

	for(i=0;i<count;i+=4)
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


