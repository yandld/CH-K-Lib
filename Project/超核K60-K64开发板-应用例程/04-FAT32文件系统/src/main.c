#include "gpio.h"
#include "uart.h"
#include "sd.h"
#include "common.h"

#include "ff.h"


/* 打印操作结果 */
#define ERROR_TRACE(rc)     do {if(rc != 0){printf("fatfs error:%d\r\n", rc);}} while(0)
uint8_t buf[32];

static FRESULT ListDirectory(const TCHAR *path);

int main(void)
{
    DelayInit();
    GPIO_QuickInit(HW_GPIOE, 6, kGPIO_Mode_OPP);
    UART_QuickInit(UART0_RX_PD06_TX_PD07, 115200);
    printf("FATFS test\r\n");
    printf("please insert SD card...\r\n");
    if(SD_QuickInit(20*1000*1000))
    {
        printf("SD card init failed!\r\n");
        while(1);
    }
    printf("SD size:%dMB\r\n", SD_GetSizeInMB());
    FRESULT rc;
    FATFS fs_sd;
    FIL fil;
    FATFS *fs;
    fs = &fs_sd;
    UINT bw,br; /* bw = byte writted br = byte readed */
    DWORD fre_clust, fre_sect, tot_sect;
    /* 挂载文件系统 */
    rc = f_mount(fs, "0:", 0);
    ERROR_TRACE(rc);
    rc = f_getfree("0:", &fre_clust, &fs);
    ERROR_TRACE(rc);
    
    ListDirectory("0:");
    
    /* 计算磁盘空间及剩余空间 */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;
    printf("%d KB total drive space.\r\n%d KB available.\r\n", tot_sect / 2, fre_sect / 2);
    /* 写入文件 */
    printf("open or create file\r\n");
    rc = f_open(&fil, "0:/fatfs.txt", FA_WRITE | FA_CREATE_ALWAYS);
    ERROR_TRACE(rc);
    printf("write file\r\n");
    rc = f_write(&fil, "HelloWorld\r\n", 12, &bw);
    ERROR_TRACE(rc);
    printf("%d bytes writen\r\n", bw);
    rc = f_close(&fil);
    /* 读取文件 */
    rc = f_open(&fil, "0:/fatfs.txt", FA_READ);
    ERROR_TRACE(rc);
    printf("file size:%d\r\n", f_size(&fil));
    printf("file contents:\r\n");
    while(1)
    {
        rc = f_read(&fil, buf, sizeof(buf), &br);
        if(rc || !br ) break;
        printf("%s", buf);
    }
    rc = f_close(&fil);
    ERROR_TRACE(rc);
    
    while(1)
    {
        GPIO_ToggleBit(HW_GPIOE, 6);
        DelayMs(500);
    }
}


static void FatfsDisplayFileInfo(FILINFO *fileInfo)
{
    char *fileName;
#if _USE_LFN
    fileName = (fileInfo->lfname[0] ? fileInfo->lfname : fileInfo->fname;
#else
    fileName = fileInfo->fname;
#endif /* _USE_LFN */
    /* note: if this file/directory don't have one attribute, '_' replace the attribute letter ('R' - readonly, 'H' - hide, 'S' - system) */
    printf("    %s - %c%c%c - %s - %dBytes - %d-%d-%d %d:%d:%d\r\n", (fileInfo->fattrib & AM_DIR) ? "dir" : "fil",
             (fileInfo->fattrib & AM_RDO) ? 'R' : '_',
             (fileInfo->fattrib & AM_HID) ? 'H' : '_',
             (fileInfo->fattrib & AM_SYS) ? 'S' : '_',
             fileName,
             (fileInfo->fsize),
             (uint32_t)((fileInfo->fdate >> 9) + 1980) /* year */,
             (uint32_t)((fileInfo->fdate >> 5) & 0x000Fu) /* month */,
             (uint32_t)(fileInfo->fdate & 0x001Fu) /* day */,
             (uint32_t)((fileInfo->ftime >> 11) & 0x0000001Fu) /* hour */,
             (uint32_t)((fileInfo->ftime >> 5) & 0x0000003Fu) /* minute */,
             (uint32_t)(fileInfo->ftime & 0x0000001Fu) /* second */
             );
}

static FRESULT ListDirectory(const TCHAR *path)
{
    FRESULT fatfsCode = FR_OK;
    FILINFO fileInfo;
    DIR dir;
    uint8_t outputLabel = 0;

#if _USE_LFN
    static uint8_t fileNameBuffer[_MAX_LFN];
    fileInfo.lfname = fileNameBuffer;
    fileInfo.lfsize = _MAX_LFN;
#endif /* _USE_LFN */

    fatfsCode = f_opendir(&dir, path);
    if (fatfsCode)
    {
        return fatfsCode;
    }
    while (1)
    {
        fatfsCode = f_readdir(&dir, &fileInfo);
        if ((fatfsCode) || (!fileInfo.fname[0]))
        {
            break;
        }
        outputLabel = 1;
        FatfsDisplayFileInfo(&fileInfo);
    }
    if (!outputLabel)
    {
        printf("\r\n");
    }

    return fatfsCode;
}
