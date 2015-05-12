


#include "parse_cmd.h"
static char rev_buf[32];

enum input_status
{
    STATUS_IDLE,
    STATUS_SOF,
    STATUS_LEN,
    STATUS_DATA,
};

void cmd_parse_handler(uint8_t ch)
{
    static enum input_status status = STATUS_IDLE;
    static int len;
    static int i;
    
    /* running status machine */
    switch(status)
    {
        case STATUS_IDLE:
            if((uint8_t)ch == 0x8A)
            {
                status = STATUS_SOF;
            }
            break;
        case STATUS_SOF:
            if((uint8_t)ch == 0x8B)
            {
                status = STATUS_LEN;
            }
            break;
        case STATUS_LEN:
            len = ch;
            status = STATUS_DATA;
            i = 0;
            break;
        case STATUS_DATA:
            
            if(i == len)
            {
                printf("frame recied!\r\n");
                status = STATUS_IDLE;
           //     g_hander->handler();
                break;
            }
            rev_buf[i++] = ch;
            break;
        default:
            break;
    }
}