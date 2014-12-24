#include "gui_appdef.h"


#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0     (GUI_ID_USER + 0x05)

extern struct rt_object_information rt_object_container[];

static const char * _aTable_1[][3] = {
  { "623499-0010001", "1", "Item 1" },
  { "623499-0010002", "2", "Item 2" },
  { "623499-0010003", "3", "Item 3" },
  { "623499-0010004", "4", "Item 4" },
  { "623499-0010005", "5", "Item 5" },
  { "623499-0010006", "6", "Item 6" },
  { "623499-0010007", "7", "Item 7" },
  { "623499-0010008", "8", "Item 8" }
};

const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
  { WINDOW_CreateIndirect,    "Dialog 1", 0,                   0,   0, 250, 270, FRAMEWIN_CF_MOVEABLE },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0,        6,  10, 210, 240, 0, 0x0, 0 },

};

void _cbDialog(WM_MESSAGE * pMsg)
{
  WM_HWIN hItem;
  int     NCode;
  int     Id;
    
    int i ;
    
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
    //
    // Initialization of 'Listview'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    LISTVIEW_AddColumn(hItem, 50, "Thread", GUI_TA_HCENTER | GUI_TA_VCENTER);
 //   LISTVIEW_AddColumn(hItem, 40, "Prioity", GUI_TA_HCENTER | GUI_TA_VCENTER);
  //  LISTVIEW_AddColumn(hItem, 40, "Stack", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddRow(hItem, NULL);
    LISTVIEW_SetGridVis(hItem, 1);
    
    struct rt_list_node *list;
    struct rt_list_node *node;
    rt_uint8_t *ptr;
    list = &rt_object_container[RT_Object_Class_Thread].object_list;
    struct rt_thread *thread;
    static const char * _aTable_2[1][3];
    for (node = list->next; node != list; node = node->next)
    {
        thread = rt_list_entry(node, struct rt_thread, list);
       // rt_kprintf("%-8.*s 0x%02x", RT_NAME_MAX, thread->name, thread->current_priority);
      //  rt_memcpy(_aTable_2[0], thread->name, 8);
      //  if (thread->stat == RT_THREAD_READY)        rt_kprintf(" ready  ");
      //  else if (thread->stat == RT_THREAD_SUSPEND) rt_kprintf(" suspend");
      //  else if (thread->stat == RT_THREAD_INIT)    rt_kprintf(" init   ");
     //   else if (thread->stat == RT_THREAD_CLOSE)   rt_kprintf(" close  ");

  //      ptr = (rt_uint8_t*)thread->stack_addr;
     //   while (*ptr == '#')ptr ++;
        LISTVIEW_AddRow(hItem, (const char **)&thread->name);
        //rt_kprintf(" 0x%08x 0x%08x 0x%08x 0x%08x %03d\n",
      //      thread->stack_size + ((rt_uint32_t)thread->stack_addr - (rt_uint32_t)thread->sp),
       //     thread->stack_size,
      //      thread->stack_size - ((rt_uint32_t) ptr - (rt_uint32_t)thread->stack_addr),
       //     thread->remaining_tick,
       //     thread->error);
    }
    // USER START (Optionally insert additional code for further widget initialization)
    // USER END
    break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

WM_HWIN _TaskerAddPageThread(void)
{
    return GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), _cbDialog, WM_UNATTACHED, 0, 0);
}

