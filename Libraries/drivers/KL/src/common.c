#include "common.h"

//!< consult io install
struct
{
    ConsultGetc_CallBackType  cgetc;
    ConsultPutc_CallBackType  cputc;
}consult_io_t;

void ConsultSelcet(ConsultGetc_CallBackType cgetc, ConsultPutc_CallBackType cputc)
{
    consult_io_t.cgetc = cgetc;
    consult_io_t.cputc = cputc;
}

#ifdef USE_STDIO
#include <stdio.h>
#if __ICCARM__
#include <yfuns.h>
#endif

#ifdef __CC_ARM // MDK Support

int fputc(int ch,FILE *f)
{
    consult_io_t.cputc(ch);
	return ch;
}

int fgetc(FILE *f)
{
    uint8_t ch;
    ch = consult_io_t.cgetc();
	return ch;
}

#elif __ICCARM__ // IAR Support
size_t __write(int handle, const unsigned char * buffer, size_t size)
{
    size_t nChars = 0;
    if (buffer == 0)
    {
        /* This means that we should flush internal buffers.  Since we*/
        /* don't we just return.  (Remember, "handle" == -1 means that all*/
        /* handles should be flushed.)*/
        return 0;
    }
    /* This function only writes to "standard out" and "standard err",*/
    /* for all other file handles it returns failure.*/
    if ((handle != _LLIO_STDOUT) && (handle != _LLIO_STDERR))
    {
        return _LLIO_ERROR;
    }
    /* Send data.*/
    while (size--)
    {
        consult_io_t.cputc(*buffer++);
        ++nChars;
    }
    return nChars;
}

#endif // Comiler Support

#endif // USE_STDIO
