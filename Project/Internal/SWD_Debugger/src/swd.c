#include "swd.h"

void SWJ_Sequence(uint32_t count, uint8_t *data)
{
    uint32_t val;
    uint32_t n;

    val = 0;
    n = 0;
    while(count--)
    {
        if (n == 0)
        {
            val = *data++;
            n = 8;
        }
    
        SW_WRITE_BIT(val & 1);
        val >>= 1;
        n--;
    }
}

uint8_t SWD_Transfer (uint32_t request, uint32_t *data)
{
    uint32_t ack;
    uint32_t bit;
    uint32_t val;
    uint32_t parity;
  
    uint32_t n;  
  
    /* Packet Request */      
    parity = 0;  
    SW_WRITE_BIT(1);         /* Start Bit */
    bit = request >> 0;        
    SW_WRITE_BIT(bit);       /* APnDP Bit */
    parity += bit;
    bit = request >> 1;        
    SW_WRITE_BIT(bit);       /* RnW Bit */  
    parity += bit;
    bit = request >> 2;        
    SW_WRITE_BIT(bit);       /* A2 Bit */   
    parity += bit;
    bit = request >> 3;        
    SW_WRITE_BIT(bit);       /* A3 Bit */   
    parity += bit;
    SW_WRITE_BIT(parity);    /* Parity Bit */            
    SW_WRITE_BIT(0);         /* Stop Bit */ 
    SW_WRITE_BIT(1);         /* Park Bit */ 
   
    /* Turnaround */           
    TMS_RD();   
    for (n = 1; n; n--)
    {     
        SW_CLOCK_CYCLE();        
    }
   
    /* Acknowledge response */ 
    bit = SW_READ_BIT();          
    ack  = bit << 0;           
    bit = SW_READ_BIT();          
    ack |= bit << 1;           
    bit = SW_READ_BIT();          
    ack |= bit << 2;           
   
    switch(ack)
    {
        case DAP_TRANSFER_OK:
            if (request & DAP_TRANSFER_RnW) /* read data */
            {
                val = 0;  
                parity = 0;            
                for (n = 32; n; n--)
                { 
                    bit = SW_READ_BIT();  /* Read RDATA[0:31] */      
                    parity += bit;       
                    val >>= 1;           
                    val  |= bit << 31;   
                }         
                bit = SW_READ_BIT();    /* Read Parity */           
                if ((parity ^ bit) & 1)
                {           
                    ack = DAP_TRANSFER_ERROR;         
                }               
                if (data) *data = val;
            
                /* Turnaround */       
                for (n = 1; n; n--)
                { 
                    SW_CLOCK_CYCLE();    
                }         
                TMS_WR();
            }
            else    /* write data */
            {
                /* Turnaround */       
                for (n = 1; n; n--) {    SW_CLOCK_CYCLE(); }
           
                TMS_WR();
                /* Write data */       
                val = *data;           
                parity = 0;            
                for (n = 32; n; n--)
                { 
                    SW_WRITE_BIT(val); /* Write WDATA[0:31] */     
                    parity += val;       
                    val >>= 1;           
                }         
                SW_WRITE_BIT(parity);/* Write Parity Bit */      
            }
            
            /* Idle cycles */               
            PIN_SWDIO_OUT(1);        
            return (ack);   
        case DAP_TRANSFER_WAIT:
        case DAP_TRANSFER_FAULT:
            /* WAIT or FAULT response */          
            if (0 && ((request & DAP_TRANSFER_RnW) != 0))
            {   
                for (n = 32+1; n; n--)
                {            
                    SW_CLOCK_CYCLE();  /* Dummy Read RDATA[0:31] + Parity */    
                }         
            }
            /* Turnaround */         
            for (n = 1; n; n--) { SW_CLOCK_CYCLE();}      
            TMS_WR();  
            if (0 && ((request & DAP_TRANSFER_RnW) == 0))
            {   
                PIN_SWDIO_OUT(0);      
                for (n = 32+1; n; n--) {    SW_CLOCK_CYCLE();  /* Dummy Write WDATA[0:31] + Parity */   };           
            }           
            PIN_SWDIO_OUT(1);        
            return (ack);  
        default:
            break;
    }
    
    /* Protocol error */       
    for (n = 1 + 32 + 1; n; n--)
    {         
        SW_CLOCK_CYCLE();      /* Back off data phase */   
    }
    PIN_SWDIO_OUT(1);          
    return (ack); 
}




//uint8_t target_unlock_sequence(void) {
//    uint32_t val;

//    // read the device ID
//    if (!swd_read_ap(MDM_IDR, &val)) {
//        return 0;
//    }
//    // verify the result
//    if (val != MCU_ID) {
//        return 0;
//    }

//    if (!swd_read_ap(MDM_STATUS, &val)) {
//        return 0;
//    }

//    // flash in secured mode
//    if (val & (1 << 2)) {
//        // hold the device in reset
//    //    target_set_state(RESET_HOLD);
//        // write the mass-erase enable bit
//        if (!swd_write_ap(MDM_CTRL, 1)) {
//            return 0;
//        }
//        while (1) {
//            // wait until mass erase is started
//            if (!swd_read_ap(MDM_STATUS, &val)) {
//                return 0;
//            }

//            if (val & 1) {
//                break;
//            }
//        }
//        // mass erase in progress
//        while (1) {            
//            // keep reading until procedure is complete
//            if (!swd_read_ap(MDM_CTRL, &val)) {
//                return 0;
//            }

//            if (val == 0) {
//                break;
//            }
//        }
//    }

//    return 1;
//}

// uint8_t swd_init_debug(void) {
//    uint32_t tmp = 0;

//    // call a target dependant function
//    // this function can do several stuff before really
//    // initing the debug
//   // target_before_init_debug();

//    if (!JTAG2SWD()) {
//        return 0;
//    }

//    if (!swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
//        return 0;
//    }

//    // Ensure CTRL/STAT register selected in DPBANKSEL
//    if (!swd_write_dp(DP_SELECT, 0)) {
//        return 0;
//    }

//    // Power up
//    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
//        return 0;
//    }

//    do {
//        if (!swd_read_dp(DP_CTRL_STAT, &tmp)) {
//            return 0;
//        }
//    } while ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) != (CDBGPWRUPACK | CSYSPWRUPACK));

//    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
//        return 0;
//    }

//    // call a target dependant function:
//    // some target can enter in a lock state
//    // this function can unlock these targets
//    target_unlock_sequence();

//    if (!swd_write_dp(DP_SELECT, 0)) {
//        return 0;
//    }

//    return 1;
//}
// 




/********************************************************************************************/

#define PULSE()             \
        DELAY();            \
        TCK_HIGH();         \
        DELAY();            \
        TCK_LOW();          \



static void SWJ_SendClock(uint32_t count, uint8_t swdio_logic)
{
    (swdio_logic)?(TMS_HIGH()):(TMS_LOW());

    while(count--)
    {
        PULSE();
    }
}


static void SWJ_SendData(uint16_t data)
{
    uint8_t i;
    
    for(i = 0; i < 16; i++)
    {
        ((data & 0x1) == 1) ? (TMS_HIGH()) : (TMS_LOW());
		PULSE();
	    data>>=1;
    }
}

static uint8_t SWJ_JTAG2SWD(void)
{
    SWJ_SendClock(51, 1);
    SWJ_SendData(0xE79E);
    SWJ_SendClock(51, 1);
    SWJ_SendClock(3, 0);
    return 0;
}

uint8_t SWJ_ReadDP(uint8_t adr, uint32_t *val)
{
    uint32_t tmp_in;
    uint8_t ack;
    uint8_t err;

    //SWJ_SendClock(8, 0);
    
    tmp_in = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(adr);
    ack = SWD_Transfer(tmp_in, val);

    (ack == DAP_TRANSFER_OK)?(err = 0):(err = 1);
    return err;
}

uint8_t SWJ_WriteDP(uint8_t adr, uint32_t val)
{
    uint32_t req;
    uint8_t ack;
    uint8_t err;
    
    req = SWD_REG_DP | SWD_REG_W | SWD_REG_ADR(adr);
    ack = SWD_Transfer(req, &val);

    (ack == DAP_TRANSFER_OK)?(err = 0):(err = 1);
    return err;
}

/* Read access port register. */
uint8_t SWJ_ReadAP(uint32_t adr, uint32_t *val)
{
    uint8_t tmp_in, ack, err;

    uint32_t apsel = adr & APSEL;
    uint32_t bank_sel = adr & APBANKSEL;

    if(SWJ_WriteDP(DP_SELECT, apsel | bank_sel))
    {
        return 1;
    }

    tmp_in = SWD_REG_AP | SWD_REG_R | SWD_REG_ADR(adr);

    /* first dummy read */
    ack = SWD_Transfer(tmp_in, val);
    ack = SWD_Transfer(tmp_in, val);

    (ack == DAP_TRANSFER_OK)?(err = 0):(err = 1);
    return err;
}


uint8_t SWJ_WriteAP(uint32_t adr, uint32_t val)
{
    uint8_t req, ack, err;
    
    uint32_t apsel = adr & APSEL;
    uint32_t bank_sel = adr & APBANKSEL;

    /* write DP select */
    if(SWJ_WriteDP(DP_SELECT, apsel | bank_sel))
    {
        return 1;
    }

    /* write AP data */
    req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(adr);
    ack = SWD_Transfer(req, &val);

    /* read DP buff */
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = SWD_Transfer(req, NULL);

    (ack == DAP_TRANSFER_OK)?(err = 0):(err = 1);
    return err;
}

/* Read 32-bit word from target memory. */
// AP CSW register, base value
#define CSW_VALUE (CSW_RESERVED | CSW_MSTRDBG | CSW_HPROT | CSW_DBGSTAT | CSW_SADDRINC)


// Write target memory.
static uint8_t swd_write_data(uint32_t address, uint32_t data) {

    uint32_t dummy;
    uint8_t req, ack;

    // put addr in TAR register
    req = SWD_REG_AP | SWD_REG_W | (1 << 2);
    ack = SWD_Transfer(req, &address);

    // write data
    req = SWD_REG_AP | SWD_REG_W | (3 << 2);
    ack = SWD_Transfer(req, &data);

    /* read DP buff */
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = SWD_Transfer(req, NULL);

//    return (ack == 0x01) ? 1 : 0;
}

uint8_t SWJ_WriteMem32(uint32_t addr, uint32_t val)
{
    
    SWJ_WriteAP(AP_CSW, CSW_VALUE | CSW_SIZE32);

    swd_write_data(addr, val);

    return 1;
}


// Read target memory.
static uint8_t SWJ_ReadData(uint32_t addr, uint32_t *val)
{
    uint8_t tmp_in[4];
    uint8_t tmp_out[4];
    uint8_t req, ack, err;

    err = SWJ_WriteAP(AP_TAR, addr);

    // read data
    req = SWD_REG_AP | SWD_REG_R | (3 << 2);
    ack = SWD_Transfer(req, val);

    // dummy read
    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = SWD_Transfer(req, val);


//    return (ack == 0x01);
}

// Write access port register
uint8_t swd_write_ap(uint32_t adr, uint32_t val) {
    uint8_t data[4];
    uint8_t req, ack;
    uint32_t apsel = adr & 0xff000000;
    uint32_t bank_sel = adr & APBANKSEL;

    SWJ_WriteDP(DP_SELECT, apsel | bank_sel);


//    switch(adr) {
//        case AP_CSW:
//            if (dap_state.csw == val)
//                return 1;
//            dap_state.csw = val;
//            break;
//        default:
//            break;
//    }

    req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(adr);
   // int2array(data, val, 4);

    if (SWD_Transfer(req, (uint32_t *)data) != 0x01) {
        return 0;
    }

    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = SWD_Transfer(req, NULL);

    return (ack == 0x01);
}


uint8_t SWJ_ReadMem32(uint32_t addr, uint32_t *val)
{
    SWJ_WriteAP(AP_CSW, CSW_VALUE | CSW_SIZE32);
    SWJ_ReadData(addr, val);
    return 1;
}

uint8_t SWJ_InitDebug(void)
{
    uint32_t tmp = 0;
    uint32_t val, err;
    
    SWJ_JTAG2SWD();
    
    if(SWJ_ReadDP(DP_IDCODE, &val))
    {
        return 1;
    }
    
    SWJ_WriteDP(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR);

    /* Ensure CTRL/STAT register selected in DPBANKSEL */
    SWJ_WriteDP(DP_SELECT, 0);

    /* Power ups */
    SWJ_WriteDP(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ);

    do
    {
        if(!SWJ_ReadDP(DP_CTRL_STAT, &tmp))
        {
            return 0;
        }
    } while ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) != (CDBGPWRUPACK | CSYSPWRUPACK));

    SWJ_WriteDP(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE);
    return 0;
}

