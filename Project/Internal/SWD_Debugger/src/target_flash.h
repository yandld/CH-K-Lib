
#ifndef __CH_SW_TARGET_FLASH_H__
#define __CH_SW_TARGET_FLASH_H__


typedef struct
{
    uint32_t init;
    uint32_t uninit;
    uint32_t erase_chip;
    uint32_t erase_sector;
    uint32_t program_page;

    FLASH_SYSCALL sys_call_param;

    uint32_t program_buffer;
    uint32_t algo_start;
    uint32_t algo_size;
    const uint32_t * image;

    uint32_t ram_to_flash_bytes_to_be_written;

} TARGET_FLASH;


typedef struct
{
    const char* name;
    const uint32_t *image;
    uint32_t algo_start;
    uint32_t algo_size;
    uint32_t flash_start;
    uint32_t flash_size;
    
}FlashAlgoType;



















#endif

