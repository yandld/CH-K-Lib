#include "ssi_cgi_handle.h"
#include "httpd.h"
#include "string.h"
#include "uart.h"
#include "stdio.h"
#include "fs.h"



#define INDEX_PAGE_SET_CGI_RSP_URL        "/index.shtml"

#define NUM_CONFIG_CGI_URIS     (sizeof(ppcURLs ) / sizeof(tCGI))
#define NUM_CONFIG_SSI_TAGS     (sizeof(ppcTags) / sizeof (char *))

static char *CGI_MessageHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] );
static int SSIHandler ( int iIndex, char *pcInsert, int iInsertLen );


static const tCGI ppcURLs[] =
{
    { "/index.shtml",           CGI_MessageHandler },
    
};

static const char *ppcTags[] =
{
    "Submit",
};

enum ssi_index_s
{
    SSI_INDEX_SUBMITCOUNT_GET = 0, //该表对应ppcTags[]的排序
} ;


//初始化ssi和cgi
void init_ssi_cgi(void)
{
    http_set_cgi_handlers(ppcURLs , NUM_CONFIG_CGI_URIS);	
    http_set_ssi_handler (SSIHandler, ppcTags, NUM_CONFIG_SSI_TAGS );
}

//*****************************************************************************
//
// This CGI handler is called whenever the web browser requests iocontrol.cgi.
//
//*****************************************************************************
static int FindCGIParameter(const char *pcToFind, char *pcParam[], int iNumParams)
{
    int iLoop;

    for(iLoop = 0; iLoop < iNumParams; iLoop++)
    {
        if(strcmp(pcToFind, pcParam[iLoop]) == 0)
        {
            return(iLoop);
        }
    }

    return(-1);
}

//清除缓冲区的内容
void  clear_response_bufer(unsigned char *buffer){
  memset(buffer,0,strlen((const char*)buffer));
}

//红灯处理函数
static char *CGI_MessageHandler( int iIndex, int iNumParams, char *pcParam[], char *pcValue[] )
{
    int  index, i;
    i = iNumParams;
    while(i--)
    {
        index = FindCGIParameter ( pcParam[i], pcParam, iNumParams );
        if(index != -1)
        {
            printf("%s:%s\r\n", pcParam[index], pcValue[index]);
        }
    }
    if(index != -1)
    {
			clear_response_bufer(data_response_buf);      //清除缓冲区的内容
    }
    return INDEX_PAGE_SET_CGI_RSP_URL;
}
//*****************************************************************************
//
// This function is called by the HTTP server whenever it encounters an SSI
// tag in a web page.  The iIndex parameter provides the index of the tag in
// the ppcTags array. This function writes the substitution text
// into the pcInsert array, writing no more than iInsertLen characters.
//
//*****************************************************************************
static int SSIHandler ( int iIndex, char *pcInsert, int iInsertLen )
{
    static uint32_t counter;
    switch(iIndex)
    {
        case SSI_INDEX_SUBMITCOUNT_GET:
        sprintf(pcInsert, "%d", counter++);
        break;
                
        default:
            strcpy( pcInsert , "??" );           
    }
 
    return strlen ( pcInsert );
}

void webserver_thread(void *parameter)
{
    httpd_init();				//初始化webserver
    init_ssi_cgi();
    while(1)
    {
        rt_thread_delay(5);
    }
}

static void webserver_start(void)
{
	rt_thread_t tid;
	tid = rt_thread_create("webserver", webserver_thread, RT_NULL, 4096, 30, 5);

	if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(webserver_start, start web server)
#endif
