#include <rtthread.h>
#include <finsh.h>
#include <dfs_posix.h>

#include "gui_appdef.h"



int cmd_calibrate(int argc, char** argv)
{
    gui_msg_t msg;
    msg.cmd = 2;
    msg.exec = TOUCH_MainTask;
    rt_mq_send(guimq, &msg, sizeof(msg));
    return 0;
}


FINSH_FUNCTION_EXPORT_ALIAS(cmd_calibrate, __cmd_calibrate, do touch calibration.);
