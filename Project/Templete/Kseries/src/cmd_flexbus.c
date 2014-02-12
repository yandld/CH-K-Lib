
#include "flexbus.h"
#include "shell/shell.h"



int CMD_FLEXBUS(int argc, char * const * argv)
{
    uint8_t instance;
    shell_printf("FLEXBUS Test CMD\r\n");
#if (defined(MK10D5))
    shell_printf("NOT SUPPORTED FEATURE\r\n");
    
#else
    FLEXBUS_InitTypeDef FLEXBUS_InitStruct;
    FLEXBUS_InitStruct.addressSpaceInByte = 512*1024;
    FLEXBUS_InitStruct.autoAckMode = kFLEXBUS_AutoAckEnable;
    FLEXBUS_InitStruct.chipSelectChl = kFLEXBUS_CS0;
    FLEXBUS_InitStruct.dataAlignMode = kFLEXBUS_DataLeftAligned;
    FLEXBUS_InitStruct.portSize = kFLEXBUS_PortSize_16Bit;
    FLEXBUS_InitStruct.chipSelectAddress = 0x60000000;
    FLEXBUS_Init(&FLEXBUS_InitStruct);
#endif

    return 0;
}



const cmd_tbl_t CommandFun_FLEXBUS = 
{
    .name = "FLEXBUS",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = CMD_FLEXBUS,
    .usage = "FLEXBUS TEST",
    .complete = NULL,
    .help = "\r\n"
};
