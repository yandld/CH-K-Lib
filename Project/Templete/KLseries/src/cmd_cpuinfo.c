#include "shell.h"
#include "clock.h"
#include "cpuidy.h"

const char* ClockSourceNameTable[] = 
{
	"CoreClock",
	"SystemClock",
	"BusClock",
	"FlexBusClock",
	"FlashClock"
};

const char* MemNameTable[] = 
{
	"ProramFlashSize",
	"DadaFlashSize",
	"FLexNVMSize",
	"EEPORMSizeInByte",
	"RAMSize",
};


int DoCPUInfo(int argc, char *const argv[])
{
    uint8_t i;
    uint32_t val;
    CPUIDY_GetPinCount(&val);
		if(argc == 1)
		{
        SHELL_printf("%-*s- %s - %dPin\r\n", 16, "CPU:", CPUIDY_GetFamID(), val);
        for(i=0;i< kClockNameCount; i++)
        {
            CLOCK_GetClockFrequency(i,&val);
            SHELL_printf("%-*s- %dKHZ\r\n", 16, ClockSourceNameTable[i],val/1000);
        }
        for(i=0;i< kMemNameCount; i++)
        {
            CPUIDY_GetMemSize(i,&val);
            SHELL_printf("%-*s- %dKB\r\n", 16, MemNameTable[i],val);
        }
    }
    if(argc == 2 && !strcmp(argv[1], "help"))
		{
			return CMD_RET_USAGE;
		}
}
 
int DoCPUComplete(int argc, char * const argv[], char last_char, int maxv, char *cmdv[])
{
    uint8_t str_len;
    uint8_t found = 0;
    uint8_t i;
    str_len = strlen(argv[argc-1]);
    switch(argc)
    {
        case 1:
            break;
        case 2:
					  if(!strncmp(argv[argc-1], "clock", str_len))
						{
                cmdv[found] = "clock";
                found++;
						}
					  if(!strncmp(argv[argc-1], "memory", str_len))
						{
                cmdv[found] = "memory";
                found++;
						}
					  if(!strncmp(argv[argc-1], "help", str_len))
						{
                cmdv[found] = "help";
                found++;
						}
            break;
        default:
            break;
    }
    return found;
}

const cmd_tbl_t CommandFun_CPU = 
{
    .name = "CPU",
    .maxargs = 2,
    .repeatable = 1,
    .cmd = DoCPUInfo,
    .usage = "display CPU clock",
    .complete = DoCPUComplete,
    .help = "\r\n"
		        "CPU clock - print CPU clock\r\n"
						"CPU memory   - print CPU memory info"
};
