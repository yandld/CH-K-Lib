#include "DIALOG.h"
#include <dfs_posix.h>
#include <stdbool.h>
#include "gui_appdef.h"


#define GUI_IMAGE_BUF_SIZE   512

#define ID_FRAMEWIN_0       (GUI_ID_USER + 0x00)
#define ID_IMAGE_0          (GUI_ID_USER + 0x04)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { FRAMEWIN_CreateIndirect, "Image", ID_FRAMEWIN_0, 0, 0, 240, 320, 0, FRAMEWIN_CF_MOVEABLE, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 50, 50, 0, IMAGE_CF_ATTACHED | IMAGE_CF_TILE | IMAGE_CF_ALPHA, 0 },
};

static unsigned char *_acBuffer = RT_NULL;
static const char *gPath;
static U32 gFileSize;
static bool gIsReadOver;

int _GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off)
{
    rt_uint32_t NumBytesRead;
    if (NumBytes >GUI_IMAGE_BUF_SIZE) NumBytes = GUI_IMAGE_BUF_SIZE;
    lseek(*(int*)p, Off, SEEK_SET);
    NumBytesRead = read(*(int*)p, _acBuffer, NumBytes);
    if(NumBytesRead == -1)
    {
        rt_kprintf("read failed\r\n");
        return NumBytes;
    }
    //rt_kprintf("NumB.ytesRead:%d Off:%d\r\n", NumBytesRead, Off);
    if(gFileSize < GUI_IMAGE_BUF_SIZE)
    {
        //rt_kprintf("close1\r\n");
        gIsReadOver = true;
        close(*(int*)(p));
    }
    else if(gFileSize - (Off + NumBytes) < 100 )
    {
        ///rt_kprintf("close2\r\n");
        gIsReadOver = true;
        close(*(int*)(p));
    }
    *ppData = _acBuffer;
    return NumBytesRead;
}


static int _DispImage(WM_HWIN Handle, const char *path)
{
    static int hFile;
    int xSize, ySize;
  //  int           nx;
  //  int           ny;
  //  int           n;
    struct stat f_stat;
    hFile = open(path, O_RDONLY , 0);
    if(hFile <0)
    {
        rt_kprintf("open file failed\r\n");
        return 1;
    }
    stat(path, &f_stat);
    gFileSize = f_stat.st_size;
    _acBuffer = rt_malloc(GUI_IMAGE_BUF_SIZE);
    read(hFile, _acBuffer, GUI_IMAGE_BUF_SIZE);
    rt_kprintf("Size:%d\r\n", gFileSize);
    gIsReadOver = false;
    switch(GUI_AppGetFileType(path))
    {
        case kBMP:
            xSize = GUI_BMP_GetXSize(_acBuffer);
            ySize = GUI_BMP_GetYSize(_acBuffer);
            IMAGE_SetBMPEx(Handle, _GetData, (void*)&hFile);
            while(gIsReadOver == false) {GUI_Exec();}
            break;
        case kJPG:
        case kJPEG:
            GUI_JPEG_INFO Info;
            if(!GUI_JPEG_GetInfoEx(_GetData, (void*)&hFile, &Info))
            {
                xSize = Info.XSize;
                ySize = Info.YSize;
                //WM_SetWindowPos(hWin, 0 ,0, xSize, ySize);
                IMAGE_SetJPEGEx(Handle, _GetData, (void*)&hFile);
               // nx = 320000 / Info.XSize;
              //  ny = 180000 / Info.YSize;
              //  (nx < ny)? (n = nx):(n = ny);
               // GUI_JPEG_DrawEx(_GetData, (void*)&fpic, 0, 0);
                rt_kprintf("xSize:%d ySize:%d\r\n", xSize, ySize);
            }
            break;
    }
    rt_free(_acBuffer);
    return 0;
}

static void _cbDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN      hItem;
    int     NCode;
//    int     Id;
//    static int _x;
    //rt_kprintf("pMsg->MsgID:%d\r\n", pMsg->MsgId);
    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        hItem = pMsg->hWin;
        FRAMEWIN_AddCloseButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
        FRAMEWIN_AddMaxButton(hItem, FRAMEWIN_BUTTON_RIGHT, 0);
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

