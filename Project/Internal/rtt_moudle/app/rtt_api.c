#include "rtt_api.h"


api_t *api;

void API_SetAddr(uint32_t addr)
{
    api = (api_t*)(addr);
}
