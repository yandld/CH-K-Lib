#include "chsw.h"


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
    SWDIO_DDR_IN();   
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
   
    if (ack == DAP_TRANSFER_OK)/* OK response */        
    {            
        /* Data transfer */      
        if (request & DAP_TRANSFER_RnW)
        {     
            /* Read data */        
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
            SWDIO_DDR_OUT();
        }
        else 
        {    
            /* Turnaround */       
            for (n = 1; n; n--) {    SW_CLOCK_CYCLE(); }
           
            SWDIO_DDR_OUT();
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
    n = 0;    
    if (n)
    {    
        PIN_SWDIO_OUT(0);      
      for (; n; n--) {  SW_CLOCK_CYCLE();   }    
    }           
    PIN_SWDIO_OUT(1);        
    return (ack);            
  }
   
  if ((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {  
    /* WAIT or FAULT response */          
    if (0 && ((request & DAP_TRANSFER_RnW) != 0)) {   
      for (n = 32+1; n; n--) {            
        SW_CLOCK_CYCLE();  /* Dummy Read RDATA[0:31] + Parity */    
      }         
    }           
    /* Turnaround */         
    for (n = 1; n; n--) {   
      SW_CLOCK_CYCLE();      
    }           
    SWDIO_DDR_OUT();  
    if (0 && ((request & DAP_TRANSFER_RnW) == 0)) {   
      PIN_SWDIO_OUT(0);      
      for (n = 32+1; n; n--) {            
        SW_CLOCK_CYCLE();  /* Dummy Write WDATA[0:31] + Parity */   
      }         
    }           
    PIN_SWDIO_OUT(1);        
    return (ack);            
  }
   
  /* Protocol error */       
  for (n = 1 + 32 + 1; n; n--) {         
    SW_CLOCK_CYCLE();      /* Back off data phase */   
  }
  PIN_SWDIO_OUT(1);          
  return (ack); 
}


uint8_t swd_read_dp(uint8_t adr, uint32_t *val)
{
    uint32_t tmp_in;
    uint8_t tmp_out[4];
    uint8_t ack;

    tmp_in = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(adr);
    ack = SWD_Transfer(tmp_in, (uint32_t *)tmp_out);

    *val = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];

    return (ack == 0x01);
}


 uint8_t swd_read_idcode(uint32_t *id)
{
    uint8_t tmp_in[1];
    uint8_t tmp_out[4];

    tmp_in[0] = 0x00;

    SWJ_Sequence(8, tmp_in);

    if (swd_read_dp(0, (uint32_t *)tmp_out) != 0x01) {
        return 0;
    }

    *id = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];

    return 1;
}

static uint8_t swd_reset(void)
{
    uint8_t tmp_in[8];
    uint8_t i = 0;
    for (i = 0; i < 8; i++)
    {
        tmp_in[i] = 0xff;
    }

    SWJ_Sequence(51, tmp_in);

    return 1;
}

static uint8_t swd_switch(uint16_t val)
{
    uint8_t tmp_in[2];

    tmp_in[0] = val & 0xff;
    tmp_in[1] = (val >> 8) & 0xff;

    SWJ_Sequence(16, tmp_in);

    return 1;
}


uint8_t JTAG2SWD(void)
{
    uint32_t tmp = 0;

    if (!swd_reset()) {
        return 0;
    }

    if (!swd_switch(0xE79E)) {
        return 0;
    }

    if (!swd_reset()) {
        return 0;
    }

    if (!swd_read_idcode(&tmp)) {
        return 0;
    }
    return 1;
}



uint8_t swd_write_dp(uint8_t adr, uint32_t val)
{
    uint32_t req;
    uint8_t data[4];
    uint8_t ack;

    req = SWD_REG_DP | SWD_REG_W | SWD_REG_ADR(adr);
    ack = SWD_Transfer(req, &val);

    return (ack == 0x01);
}


uint8_t swd_read_ap(uint32_t adr, uint32_t *val) {
    uint8_t tmp_in, ack;
    uint8_t tmp_out[4];

    uint32_t apsel = adr & 0xff000000;
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    tmp_in = SWD_REG_AP | SWD_REG_R | SWD_REG_ADR(adr);

    // first dummy read
    SWD_Transfer(tmp_in, (uint32_t *)tmp_out);
    ack = SWD_Transfer(tmp_in, (uint32_t *)tmp_out);

    *val = (tmp_out[3] << 24) | (tmp_out[2] << 16) | (tmp_out[1] << 8) | tmp_out[0];

    return (ack == 0x01);
}

uint8_t swd_write_ap(uint32_t adr, uint32_t val)
{
    uint8_t data[4];
    uint8_t req, ack;
    uint32_t apsel = adr & 0xff000000;
    uint32_t bank_sel = adr & APBANKSEL;

    if (!swd_write_dp(DP_SELECT, apsel | bank_sel)) {
        return 0;
    }

    req = SWD_REG_AP | SWD_REG_W | SWD_REG_ADR(adr);

    if (SWD_Transfer(req, (uint32_t *)&val) != 0x01) {
        return 0;
    }

    req = SWD_REG_DP | SWD_REG_R | SWD_REG_ADR(DP_RDBUFF);
    ack = SWD_Transfer(req, NULL);

    return (ack == 0x01);
}

uint8_t target_unlock_sequence(void) {
    uint32_t val;

    // read the device ID
    if (!swd_read_ap(MDM_IDR, &val)) {
        return 0;
    }
    // verify the result
    if (val != MCU_ID) {
        return 0;
    }

    if (!swd_read_ap(MDM_STATUS, &val)) {
        return 0;
    }

    // flash in secured mode
    if (val & (1 << 2)) {
        // hold the device in reset
    //    target_set_state(RESET_HOLD);
        // write the mass-erase enable bit
        if (!swd_write_ap(MDM_CTRL, 1)) {
            return 0;
        }
        while (1) {
            // wait until mass erase is started
            if (!swd_read_ap(MDM_STATUS, &val)) {
                return 0;
            }

            if (val & 1) {
                break;
            }
        }
        // mass erase in progress
        while (1) {            
            // keep reading until procedure is complete
            if (!swd_read_ap(MDM_CTRL, &val)) {
                return 0;
            }

            if (val == 0) {
                break;
            }
        }
    }

    return 1;
}

 uint8_t swd_init_debug(void) {
    uint32_t tmp = 0;

    // call a target dependant function
    // this function can do several stuff before really
    // initing the debug
   // target_before_init_debug();

    if (!JTAG2SWD()) {
        return 0;
    }

    if (!swd_write_dp(DP_ABORT, STKCMPCLR | STKERRCLR | WDERRCLR | ORUNERRCLR)) {
        return 0;
    }

    // Ensure CTRL/STAT register selected in DPBANKSEL
    if (!swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    // Power up
    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ)) {
        return 0;
    }

    do {
        if (!swd_read_dp(DP_CTRL_STAT, &tmp)) {
            return 0;
        }
    } while ((tmp & (CDBGPWRUPACK | CSYSPWRUPACK)) != (CDBGPWRUPACK | CSYSPWRUPACK));

    if (!swd_write_dp(DP_CTRL_STAT, CSYSPWRUPREQ | CDBGPWRUPREQ | TRNNORMAL | MASKLANE)) {
        return 0;
    }

    // call a target dependant function:
    // some target can enter in a lock state
    // this function can unlock these targets
    target_unlock_sequence();

    if (!swd_write_dp(DP_SELECT, 0)) {
        return 0;
    }

    return 1;
}
