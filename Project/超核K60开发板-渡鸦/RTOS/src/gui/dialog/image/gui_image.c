#include "gui_appdef.h"


#define GUI_IMAGE_BUF_SIZE   (1024)

#define ID_FRAMEWIN_0       (GUI_ID_USER + 0x00)
#define ID_IMAGE_0          (GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "Image", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, FRAMEWIN_CF_MOVEABLE, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 50, 50, 0, IMAGE_CF_ATTACHED | IMAGE_CF_TILE | IMAGE_CF_ALPHA, 0 },
};

static unsigned char *gBuffer = RT_NULL;
static const char *gPath;

int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off)
{
    int hFile;
    rt_uint32_t NumBytesRead;
    hFile = open(gPath, O_RDONLY , 0);
    if(hFile < 0)
    {
        rt_kprintf("open failed\r\n");
        return NumBytes;
    }
    
    if (NumBytes >GUI_IMAGE_BUF_SIZE) NumBytes = GUI_IMAGE_BUF_SIZE;
    lseek(hFile, Off, SEEK_SET);
    NumBytesRead = read(hFile, gBuffer, NumBytes);
    close(hFile);
    if(NumBytesRead == -1)
    {
        rt_kprintf("read failed\r\n");
        return NumBytes;
    }
 //   rt_kprintf("NumB.ytesRead:%d Off:%d\r\n", NumBytesRead, Off);
    *ppData = gBuffer;
    return NumBytesRead;
}


static int _DispImage(WM_HWIN Handle, const char *path)
{
    int hFile;
    int xSize, ySize;
    hFile = open(path, O_RDONLY , 0);
    if(hFile < 0)
    {
        rt_kprintf("open file failed\r\n");
        return 1;
    }

    gBuffer = rt_malloc(GUI_IMAGE_BUF_SIZE);
    read(hFile, gBuffer, GUI_IMAGE_BUF_SIZE);
   // rt_kprintf("Size:%d\r\n", gFileSize);
    close(hFile);
    switch(GUI_AppGetFileType(path))
    {
        case kBMP:
            xSize = GUI_BMP_GetXSize(gBuffer);
            ySize = GUI_BMP_GetYSize(gBuffer);
            IMAGE_SetBMPEx(Handle, _GetData, (void*)&hFile);
            break;
        case kJPG:
        case kJPEG:
            GUI_JPEG_INFO Info;
            if(!GUI_JPEG_GetInfoEx(_GetData, (void*)&hFile, &Info))
            {
                xSize = Info.XSize;
                ySize = Info.YSize;
                IMAGE_SetJPEGEx(Handle, _GetData, (void*)&hFile);
                rt_kprintf("xSize:%d ySize:%d\r\n", xSize, ySize);
            }
            break;
        default:
            rt_kprintf("Unknown Type:%s\r\n", path);
            break;
    }
    GUI_Delay(1000);
    rt_free(gBuffer);
    return 0;
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN      hItem;
    int     NCode;
//int     Id;
//static int _x;
//rt_kprintf("pMsg->MsgID:%d\r\n", pMsg->MsgId);
    switch (pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
            hItem = pMsg->hWin;
            FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
            //FRAMEWIN_AddMaxButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
            WM_MakeModal(hItem);
        
            hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
            //SCROLLBAR_CreateAttached(hItem, SCROLLBAR_CF_VERTICAL);
            //SCROLLBAR_CreateAttached(hItem, 0);
            _DispImage(hItem, gPath);
            //WM_MoveWindow(hItem, 100, 100);
            break;
        default:
            WM_DefaultProc(pMsg);
        break;
    }
}

void GUI_AppDispImage(const char * path)
{
    gPath = path;
    GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return;
}


#ifdef FINSH_USING_MSH
#include "finsh.h"
extern char working_directory[];

int ui_image(int argc, char** argv)
{
    gui_msg_t msg;
    char *fullpath;
    if(argc != 2)
    {
        return -1;
    }
     
    fullpath = dfs_normalize_path(NULL, argv[1]);

    msg.cmd = 2;
    msg.exec = GUI_AppDispImage;
    msg.parameter = (void *)fullpath;
    rt_mq_send(guimq, &msg, sizeof(msg));
    return 0;
}

MSH_CMD_EXPORT(ui_image, show a picture file.);
#endif
