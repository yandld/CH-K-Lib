#include "bl_bsp.h"
#include "bl_core.h"
#include "bl_cfg.h"

/*
 *
 *  Definitions
 *  
 */
 
 typedef int32_t status_t;
 
 enum
 {
     kCallbackBufferSize = 64,
     kPacket_MaxPayloadSize = 32,
 };
 
 //! @brief Serial framing packet constants.
enum _framing_packet_constants
{
    kFramingPacketStartByte         = 0x5a,
    kFramingPacketType_Ack          = 0xa1,
    kFramingPacketType_Nak          = 0xa2,
    kFramingPacketType_AckAbort     = 0xa3,
    kFramingPacketType_Command      = 0xa4,
    kFramingPacketType_Data         = 0xa5,
    kFramingPacketType_Ping         = 0xa6,
    kFramingPacketType_PingResponse = 0xa7
};
 
 enum
 {
     kPacketState_StartByte,
     kPacketState_PacketTye,
     kPacketState_Length,
     kPacketState_CRC,
     kPacketState_Payload
 };
 
enum 
{
    kStatus_Success             = 0,
    kStatus_Fail                = 1,
    kStatus_Busy                = 15,
    kStatus_UnknownProperty     = 10300,
    
    kStatus_FlashAlignmentError = 101,
    kStatus_FlashAccessError = 103,
    kStatus_FlashProtectionViolation = 104,
    kStatus_FlashCommandFailure = 105,
};
 
typedef struct _serial_packet
{
    uint8_t startByte;
    uint8_t packetType;
    uint8_t lengthInBytes[2];
    uint8_t crc16[2];
    uint8_t payload[32];
}serial_packet_t;

typedef enum 
{
    kCommandPhase_Command,
    kCommandPhase_Data,
}command_state_t;

typedef struct
{
    volatile uint32_t writeOffset;
    uint32_t readOffset;
    uint8_t callbackBuffer[kCallbackBufferSize];
    
    bool isAckNeeded;
    bool isAckAbortNeeded;
}serial_context_t;


typedef struct
{
    command_state_t state;
    
    uint8_t *data;
    uint32_t address;
    uint32_t count;
    uint8_t commandTag;
    uint8_t option;   
}bootloader_context_t;
 
//! @name Flash controller command numbers
//@{
#define FTFx_VERIFY_BLOCK                  0x00 //!< RD1BLK
#define FTFx_VERIFY_SECTION                0x01 //!< RD1SEC
#define FTFx_PROGRAM_CHECK                 0x02 //!< PGMCHK
#define FTFx_READ_RESOURCE                 0x03 //!< RDRSRC
#define FTFx_PROGRAM_LONGWORD              0x06 //!< PGM4
#define FTFx_PROGRAM_PHRASE                0x07 //!< PGM8
#define FTFx_ERASE_BLOCK                   0x08 //!< ERSBLK
#define FTFx_ERASE_SECTOR                  0x09 //!< ERSSCR
#define FTFx_PROGRAM_SECTION               0x0B //!< PGMSEC
#define FTFx_VERIFY_ALL_BLOCK              0x40 //!< RD1ALL
#define FTFx_READ_ONCE                     0x41 //!< RDONCE
#define FTFx_PROGRAM_ONCE                  0x43 //!< PGMONCE
#define FTFx_ERASE_ALL_BLOCK               0x44 //!< ERSALL
#define FTFx_SECURITY_BY_PASS              0x45 //!< VFYKEY
#define FTFx_ERASE_ALL_BLOCK_UNSECURE      0x49 //!< ERSALLU
#define FTFx_SWAP_CONTROL                  0x46 //!< SWAP
#define FTFx_SET_FLEXRAM_FUNCTION          0x81 //!< SETRAM
//@}


/*
 *   Local functions
 */
static void application_run(void);
static status_t serial_packet_read(uint8_t packetType);
static uint8_t read_byte(void);
static uint16_t calculate_crc(serial_packet_t *packet);
static void crc16_update(uint16_t *currectCrc, const uint8_t *src, uint32_t lengthInBytes);


/*
 *   Local variables
 */
static serial_context_t s_serialContext;
static serial_packet_t s_readPacket;
static serial_packet_t s_writePacket;
static const uint8_t s_pingPacket[] = {kFramingPacketStartByte, kFramingPacketType_PingResponse, 0, 2, 1, 'P', 0, 0, 0xaa, 0xea};
static const uint8_t s_packet_ack[] = {kFramingPacketStartByte, kFramingPacketType_Ack};
static const uint8_t s_packet_abort[] = {kFramingPacketStartByte, kFramingPacketType_AckAbort};
static bootloader_context_t bl_ctx;

volatile uint32_t* const kFCCOBx = 
#if defined(FTFE)
    (volatile uint32_t*)&FTFE->FCCOB3;
#elif defined (FTEL)
    (volatile uint32_t*)&FTFL->FCCOB3;
#elif defined (FTFA)
    (volatile uint32_t*)&FTFA->FCCOB3;
#endif

#if defined (FTFE)
#define FTFx    FTFE
#define BL_FEATURE_PROGRAM_PHASE    1
#elif defined (FTFL)
#define FTFx    FTFL
#define BL_FEATURE_PROGRAM_PHASE    0
#elif defined (FTFA)
#define FTFx    FTFA
#define BL_FEATURE_PROGRAM_PHASE    0
#endif // 

#define FTFx_FSTAT_CCIF_MASK        0x80u
#define FTFx_FSTAT_RDCOLERR_MASK    0x40u
#define FTFx_FSTAT_ACCERR_MASK      0x20u
#define FTFx_FSTAT_FPVIOL_MASK      0x10u
#define FTFx_FSTAT_MGSTAT0_MASK     0x01u

/*
 *  Functions
 */

void crc16_update(uint16_t *currectCrc, const uint8_t *src, uint32_t lengthInBytes)
{
    uint32_t crc = *currectCrc;
    uint32_t j;
    for (j=0; j < lengthInBytes; ++j)
    {
        uint32_t i;
        uint32_t byte = src[j];
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= 0x1021;
            }
            crc = temp;
        }
    }
}

uint16_t calculate_crc(serial_packet_t *packet)
{
    uint16_t crc_val = 0;
    uint8_t *start = (uint8_t*)packet;
    uint16_t payloadLength;
    crc16_update(&crc_val, start, 4);
    start = (uint8_t*)&packet->payload[0];
    payloadLength = *(uint16_t*)&packet->lengthInBytes[0];
    crc16_update(&crc_val, start,  payloadLength);
    
    return crc_val;
}

void bootloader_data_sink(uint8_t byte)
{
    s_serialContext.callbackBuffer[s_serialContext.writeOffset++] = byte;
    s_serialContext.writeOffset &= (kCallbackBufferSize-1);
}

uint8_t read_byte(void)
{
    s_serialContext.readOffset &= kCallbackBufferSize-1;
    while(s_serialContext.readOffset == s_serialContext.writeOffset)
    {
    }
    return s_serialContext.callbackBuffer[s_serialContext.readOffset++];
}

status_t serial_packet_read(uint8_t packetType)
{
    static int32_t s_packetState = kPacketState_StartByte;
    static uint32_t s_stateCnt;
    uint16_t *packetLength;
    bool isPacketComplete = false;
    uint8_t tmp = read_byte();
    switch(s_packetState)
    {
        case kPacketState_StartByte:
            if (tmp == kFramingPacketStartByte)
            {
                s_readPacket.startByte = kFramingPacketStartByte;
                s_packetState = kPacketState_PacketTye;
            }
            break;
        case kPacketState_PacketTye:
            if (tmp == kFramingPacketType_Ping)
            {
                // Send ping packet here.
                bl_hw_if_write((void*)s_pingPacket, sizeof(s_pingPacket));
                s_packetState = kPacketState_StartByte;
            }
            else if(tmp == packetType)
            {
                s_readPacket.packetType = packetType;
                s_packetState = kPacketState_Length;
                s_stateCnt = 0;
            }
            else
            {
                s_packetState = kFramingPacketStartByte;
            }
            break;
        case kPacketState_Length:
            s_readPacket.lengthInBytes[s_stateCnt++] = tmp;
            if (s_stateCnt >= 2)
            {
                s_packetState = kPacketState_CRC;
                s_stateCnt = 0;
                
                packetLength = (uint16_t*)&s_writePacket.lengthInBytes[0];
                if (*packetLength > kPacket_MaxPayloadSize)
                {
                    *packetLength = kPacket_MaxPayloadSize;
                }
            }
            break;
        case kPacketState_CRC:
            s_readPacket.crc16[s_stateCnt++] = tmp;
            if (s_stateCnt >= 2)
            {
                s_packetState = kPacketState_Payload;
                s_stateCnt = 0;
            }
            break;
        case kPacketState_Payload:
            s_readPacket.payload[s_stateCnt++] = tmp;
            if (s_stateCnt >= *(uint16_t*)&s_writePacket.lengthInBytes[0])
            {
                s_packetState = kPacketState_StartByte;
                isPacketComplete = true; 
            }
            break;
    }
    
    if (isPacketComplete)
    {
        uint16_t calculatedCrc = calculate_crc(&s_writePacket);
        uint16_t receivedCrc = *(uint16_t*)&s_writePacket.lengthInBytes[0];
        
        if (calculatedCrc == receivedCrc)
        {
            s_serialContext.isAckNeeded = true;
            __disable_irq();
            return kStatus_Success;
        }
        else 
        {
            return kStatus_Fail;
        }
    }
    else
    {
        return kStatus_Busy;
    }
}




status_t serial_packet_write(void)
{
    uint16_t packetLength;
    uint16_t crc16;
    uint8_t startByte;
    uint8_t packetType;
    command_packet_t *cmdPacket;
    
    
    __enable_irq();
    if (s_serialContext.isAckNeeded)
    {
        s_serialContext.isAckNeeded = false;
        bl_hw_if_write(s_packet_ack, sizeof(s_packet_ack));
    }
    if (s_serialContext.isAckAbortNeeded)
    {
        s_serialContext.isAckAbortNeeded = false;
        bl_hw_if_write(s_packet_abort, sizeof(s_packet_abort));
    }
    
    cmdPacket = (command_packet_t*)&s_writePacket.payload[0];
    
    s_writePacket.startByte = kFramingPacketStartByte;
    s_writePacket.packetType = kFramingPacketType_Command;
    packetLength = (1 + cmdPacket->parameterCount) * sizeof(uint32_t);
    
    *(uint16_t*)&s_writePacket.lengthInBytes[0] = packetLength;
    crc16 = calculate_crc(&s_writePacket);
    *(uint16_t*)&s_writePacket.crc16[0] = crc16;
    
    bl_hw_if_write((void*)&s_writePacket, 6 + packetLength);
    
    // Wait ACK
    startByte = read_byte();
    packetType = read_byte();
    
    if (startByte == kFramingPacketStartByte && packetType == kFramingPacketType_Ack)
    {
        return kStatus_Success;
    }
    return kStatus_Fail;
}

static status_t handle_get_property(void)
{
    get_property_packet_t *getPropertyPkt = (get_property_packet_t*)&s_readPacket.payload[0];
    get_property_response_packet_t *packet = (get_property_response_packet_t*)&s_writePacket;
    uint32_t propertySize = 1;
    
    packet->commandPacket.commandTag = kCommandTag_GetPropertyResponse;
    packet->commandPacket.flags = 0;
    packet->commandPacket.parameterCount = 1;
    packet->status = kStatus_Success;
    
    switch(getPropertyPkt->propertyTag)
    {
        case kPropertyTag_BootloaderVersion:
            packet->propertyValue[0] = 0x00020149;  // K1.2.0
            break;
        case kPropertyTag_AvailablePeripherals:
            packet->propertyValue[0] = 0x01; // Only UART peripheral is available;
            break;
        case kPropertyTag_FlashStartAddress:
            packet->propertyValue[0] = 0x00;
            break;
        case kPropertyTag_FlashSizeInBytes:
            packet->propertyValue[0] = 0x20000; // 128KB;
            break;
        case kPropertyTag_AvailableCommands:
            packet->propertyValue[0] = kCommandTag_FlashEraseRegion |
                                       kCommandTag_WriteMemory |
                                       kCommandTag_GetProperty |
                                       kCommandTag_Reset;
            break;
        case kPropertyTag_MaxPacketSize:
            packet->propertyValue[0] = kPacket_MaxPayloadSize;
            break;
        case kPropertyTag_ReservedRegions:
            propertySize = 4;
            packet->propertyValue[0] = 0x20000000;
            packet->propertyValue[1] = 0x200003FF;
            packet->propertyValue[2] = 0;
            packet->propertyValue[3] = 0xFFF;
            break;
        case kPropertyTag_FlashSecurityState:
            packet->propertyValue[0] = 0;
            
            break;
        default:
            propertySize = 0;
            packet->status = kStatus_UnknownProperty;
            break;
    }
    packet->commandPacket.parameterCount += propertySize;
    
    return serial_packet_write();
}

status_t send_generic_response(uint32_t commandTag, uint32_t status)
{
    generic_response_packet_t *packet = (generic_response_packet_t*)&s_writePacket.payload[0];
    packet->commandPacket.commandTag = kCommandTag_GenericResponse;
    packet->commandPacket.flags = 0;
    packet->commandPacket.reserved = 0;
    packet->commandPacket.parameterCount = 2;
    packet->commandTag = commandTag;
    packet->status = status;
    
    return serial_packet_write();
}

static status_t flash_command_sequence(void)
{
    uint8_t fstat;
    status_t status = kStatus_Success;
    FTFx->FSTAT = (FTFx_FSTAT_RDCOLERR_MASK | FTFx_FSTAT_ACCERR_MASK | FTFx_FSTAT_FPVIOL_MASK);
    FTFx->FSTAT = FTFx_FSTAT_CCIF_MASK;
    
    while(!(FTFx->FSTAT & FTFx_FSTAT_CCIF_MASK))
    {
    }
    fstat = FTFx->FSTAT;
    
    if (fstat & FTFx_FSTAT_ACCERR_MASK)
    {
        status = kStatus_FlashAccessError;
    }
    else if(fstat & FTFx_FSTAT_FPVIOL_MASK)
    {
        status = kStatus_FlashProtectionViolation;
    }
    else if (fstat & FTFx_FSTAT_MGSTAT0_MASK)
    {
        status = kStatus_FlashCommandFailure;
    }
    
    return status;
}

static status_t flash_erase(uint32_t start, uint32_t length)
{
    uint32_t alignedStart;
    uint32_t alignedLength;
    status_t status = kStatus_Success;
    alignedStart = start & (~BL_FLASH_SECTOR_SIZE);
    alignedLength = (start - alignedStart + length + BL_FLASH_SECTOR_SIZE-1) & (~BL_FLASH_SECTOR_SIZE); 
    
    while(alignedLength)
    {
        kFCCOBx[0] = alignedStart;
        FTFx->FCCOB0 = FTFx_ERASE_SECTOR;
        status = flash_command_sequence();
        if (status != kStatus_Success)
        {
            break;
        }
        alignedStart += BL_FLASH_SECTOR_SIZE;
        alignedLength -= BL_FLASH_SECTOR_SIZE;
    }
    
    return status;
}

static status_t handle_flash_erase_region(void)
{
    status_t status = kStatus_Success;
    flash_erase_region_packet_t *packet = (flash_erase_region_packet_t*)&s_writePacket.payload[0];
    
    status = flash_erase(packet->startAddress, packet->byteCount); 
    
    return send_generic_response(kCommandTag_FlashEraseRegion, status);
}

static status_t handle_write_memory(void)
{
    write_memory_packet_t *packet = (write_memory_packet_t*)&s_writePacket.payload[0];
    
    bl_ctx.address = packet->startAddress;
    bl_ctx.count = packet->byteCount;
    return send_generic_response(kCommandTag_WriteMemory, kStatus_Success);
}

static status_t handle_reset(void)
{
    send_generic_response(kCommandTag_Reset, kStatus_Success);
    
    NVIC_SystemReset();
    
    return kStatus_Success;
}

static status_t flash_program(uint32_t start, uint32_t *src, uint32_t length)
{
    status_t status = kStatus_Success;
    uint8_t *byteStart = (uint8_t*)src;
    uint32_t i;
#if BL_FEATURE_PROGRAM_PHASE
    uint8_t alignmentSize = 8;
#else
    uint8_t alignmentSize = 4;
#endif 
    if (start & (alignmentSize - 1))
    {
        status = kStatus_FlashAlignmentError;
    }
    else
    {
        while(length)
        {
            if (length < alignmentSize)
            {
                for(i=length; i<alignmentSize; i++)
                {
                    byteStart[i] = 0xFF;
                }
            }
            kFCCOBx[0] = start;
            kFCCOBx[1] = *src++;
#if BL_FEATURE_PROGRAM_PHASE
            kFCCOBx[2] = *src++;
#endif 
            status = flash_command_sequence();
            if (status != kStatus_Success)
            {
                break;
            }
            start += alignmentSize;
            length -= (length > alignmentSize) ? alignmentSize: length;
        }
    }
    
    
    return kStatus_Success;
}

static void finalize_data_phase(void)
{
    send_generic_response(kCommandTag_WriteMemory, kStatus_Success);
}

static void data_phase_abort(void)
{
   s_serialContext.isAckAbortNeeded = true;
   s_serialContext.isAckNeeded = false;
}

static status_t handle_data_phase(bool *hasMoreData)
{
    uint16_t packetLength = *(uint16_t*)&s_readPacket.lengthInBytes[0];
    uint32_t *src = (uint32_t*)&s_readPacket.payload[0];
    uint32_t writeLength;
    status_t status = kStatus_Success;
    if(bl_ctx.count)
    {
        writeLength = (packetLength < bl_ctx.count) ? packetLength : bl_ctx.count;
        {
            status = flash_program(bl_ctx.address, src, writeLength);
            if (status != kStatus_Success)
            {
                data_phase_abort();
                finalize_data_phase();
                *hasMoreData = false;
                return kStatus_Success;
            }
        }
        bl_ctx.count -= writeLength;
        bl_ctx.address += writeLength;
    }
    
    if (bl_ctx.count)
    {
        *hasMoreData = true;
    }
    else
    {
        *hasMoreData = false;
        finalize_data_phase();
    }
    
    return kStatus_Success;
}


void bootloader_run(void)
{
    command_packet_t *commandPkt;
    status_t status;
    bool hasMoreData = false;
    while(1)
    {
        status = serial_packet_read(kFramingPacketType_Command);
        if (status != kStatus_Success)
        {
            continue;
        }
        
        switch(bl_ctx.state)
        {
            case kCommandPhase_Command:
                commandPkt = (command_packet_t*)&s_readPacket.payload[0];
                switch(commandPkt->commandTag)
                {
                    case kCommandTag_GetProperty:
                        handle_get_property();
                        break;
                    case kCommandTag_FlashEraseRegion:
                        handle_flash_erase_region();
                        break;
                    case kCommandTag_WriteMemory:
                        handle_write_memory();
                        bl_ctx.state = kCommandPhase_Data;
                        break;
                    case kCommandTag_Reset:
                        handle_reset();
                        break;
                }
                break;
            case kCommandPhase_Data:
                status = handle_data_phase(&hasMoreData);
                if (status != kStatus_Success)
                {
                    bl_ctx.state = kCommandPhase_Command;
                }
                else
                {
                    if (!hasMoreData)
                    {
                        bl_ctx.state = kCommandPhase_Command;
                    }                        
                }
                break;
            default:
                break;
        }
    }
}

void application_run(void)
{
    typedef void(*app_entry_t)(void);
    
    static uint32_t s_stackPointer = 0;
    static uint32_t s_applicationEntry = 0;
    static app_entry_t s_application = 0;
    
    uint32_t* vectorTable = (uint32_t*)APPLICATION_BASE;
    s_stackPointer = vectorTable[0];
    s_applicationEntry = vectorTable[1];
    s_application = (app_entry_t)s_applicationEntry;
    
    // Change MSP and PSP
    __set_MSP(s_stackPointer);
    __set_PSP(s_stackPointer);
    
    // Jump to application
    s_application();
    
    // Should never reach here.
    __NOP();
}
