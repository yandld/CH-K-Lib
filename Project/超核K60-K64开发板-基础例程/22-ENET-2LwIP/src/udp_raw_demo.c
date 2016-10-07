#include "lwip/mem.h"
#include "lwip/raw.h"
#include "lwip/udp.h"
#include "lwip/icmp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/sys.h"
#include "lwip/timers.h"
#include "lwip/inet_chksum.h"
#include "lwip/init.h"
#include "netif/etharp.h"

#define UDP_PORT        (7)

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
    int i;
    static u8_t buffer[1024];
    if (p != NULL) 
    {
        if(pbuf_copy_partial(p, buffer, p->tot_len, 0) != p->tot_len) 
        {
            LWIP_DEBUGF(LWIP_DBG_ON, ("pbuf_copy_partial failed\r\n"));
        } 
        else 
        {
            printf("port:%d\r\n", port);
            for(i=0; i<p->tot_len; i++)
            {
                printf("%c", buffer[i]);
            }
            printf("\r\n");
            
            // send received packet back to sender
            udp_sendto(pcb, p, addr, port);
        }

        // free the pbuf
        pbuf_free(p);
    }
}

void udp_demo_init(void)
{
    struct udp_pcb * pcb;

    // get new pcb
    pcb = udp_new();
    if (pcb == NULL) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
        return;
    }

    // bind to any IP address on port
    if (udp_bind(pcb, IP_ADDR_ANY, UDP_PORT) != ERR_OK) {
        LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
        return;
    }
    printf("UDP demo: bind to PORT:%d OK!\r\n", UDP_PORT);

    // set udp_echo_recv() as callback function
    // for received packets
    udp_recv(pcb, udp_echo_recv, NULL);
}
