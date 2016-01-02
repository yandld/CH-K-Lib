#ifndef __BL_CORE_H__
#define __BL_CORE_H__

#include <stdint.h>


//! @brief Commands codes.
enum _command_tags {
    kCommandTag_GenericResponse             = 0xa0,
    kCommandTag_FlashEraseAll               = 0x01,
    kCommandTag_FlashEraseRegion            = 0x02,
    kCommandTag_ReadMemory                  = 0x03,
    kCommandTag_ReadMemoryResponse          = 0xa3,
    kCommandTag_WriteMemory                 = 0x04,
    kCommandTag_FillMemory                  = 0x05,
    kCommandTag_FlashSecurityDisable        = 0x06,
    kCommandTag_GetProperty                 = 0x07,
    kCommandTag_GetPropertyResponse         = 0xa7,
    kCommandTag_ReceiveSbFile               = 0x08,
    kCommandTag_Execute                     = 0x09,
    kCommandTag_Call                        = 0x0a,
    kCommandTag_Reset                       = 0x0b,
    kCommandTag_SetProperty                 = 0x0c,
    kCommandTag_FlashEraseAllUnsecure       = 0x0d,
    kCommandTag_FlashProgramOnce            = 0x0e,
    kCommandTag_FlashReadOnce               = 0x0f,
    kCommandTag_FlashReadOnceResponse       = 0xaf,
    kCommandTag_FlashReadResource           = 0x10,
    kCommandTag_FlashReadResourceResponse   = 0xb0,
    kCommandTag_ConfigureQuadSpi            = 0x11,

    kFirstCommandTag                    = kCommandTag_FlashEraseAll,

    //! Maximum linearly incrementing command tag value, excluding the response commands.
    kLastCommandTag                     = kCommandTag_ConfigureQuadSpi,

    kResponseCommandHighNibbleMask = 0xa0           //!< Mask for the high nibble of a command tag that identifies it as a response command.
};

//! @brief Command packet format.
typedef struct CommandPacket
{
    uint8_t commandTag;         //!< A command tag.
    uint8_t flags;              //!< Combination of packet flags.
    uint8_t reserved;           //!< Reserved, helpful for alignment, set to zero.
    uint8_t parameterCount;     //!< Number of parameters that follow in buffer.
} command_packet_t;

//! @name Command Packet formats
//@{

//! @brief FlashEraseRegion packet format.
typedef struct FlashEraseRegionPacket
{
    command_packet_t commandPacket; //!< header
    uint32_t startAddress;          //!< Paremeter 0: start address.
    uint32_t byteCount;             //!< Parameter 1: number of bytes.
} flash_erase_region_packet_t;

//! @brief Property tags.
//! @note Do not change any tag values. Add tags at the end.
enum _property_tag
{
    kPropertyTag_ListProperties          = 0x00,
    kPropertyTag_BootloaderVersion       = 0x01,
    kPropertyTag_AvailablePeripherals    = 0x02,
    kPropertyTag_FlashStartAddress       = 0x03,
    kPropertyTag_FlashSizeInBytes        = 0x04,
    kPropertyTag_FlashSectorSize         = 0x05,
    kPropertyTag_FlashBlockCount         = 0x06,
    kPropertyTag_AvailableCommands       = 0x07,
    kPropertyTag_CrcCheckStatus          = 0x08,
    kPropertyTag_Reserved9               = 0x09,
    kPropertyTag_VerifyWrites            = 0x0a,
    kPropertyTag_MaxPacketSize           = 0x0b,
    kPropertyTag_ReservedRegions         = 0x0c,
    kPropertyTag_ValidateRegions         = 0x0d,
    kPropertyTag_RAMStartAddress         = 0x0e,
    kPropertyTag_RAMSizeInBytes          = 0x0f,
    kPropertyTag_SystemDeviceId          = 0x10,
    kPropertyTag_FlashSecurityState      = 0x11,
    kPropertyTag_UniqueDeviceId          = 0x12,
    kPropertyTag_FacSupport              = 0x13,
    kPropertyTag_FlashAccessSegmentSize  = 0x14,
    kPropertyTag_FlashAccessSegmentCount = 0x15,
    kPropertyTag_FlashReadMargin         = 0x16,
    kPropertyTag_QspiInitStatus          = 0x17,
    kPropertyTag_InvalidProperty         = 0xFF,
};

enum
{
    kMaxPropertyReturnValues = 6,
};

//! @brief GetProperty packet format.
typedef struct GetPropertyPacket
{
    command_packet_t commandPacket; //!< header
    uint32_t propertyTag;           //!< Paremeter 0: requested property tag.
} get_property_packet_t;

//! @brief Generic response packet format.
typedef struct GenericResponsePacket
{
    command_packet_t commandPacket; //!< header
    uint32_t status;                //!< parameter 0
    uint32_t commandTag;            //!< parameter 1
} generic_response_packet_t;

//! @brief Get Property response packet format.
typedef struct GetPropertyResponsePacket
{
    command_packet_t commandPacket; //!< header
    uint32_t status;                //!< parameter 0
    uint32_t propertyValue[kMaxPropertyReturnValues]; //!< up to 6 other parameters
} get_property_response_packet_t;

//! @brief FlashSecurityDisable packet format.
typedef struct FlashSecurityDisablePacket
{
    command_packet_t commandPacket; //!< header
    uint32_t keyLow;                //!< Paremeter 0: key bytes 0-3.
    uint32_t keyHigh;               //!< Parameter 1: key bytes 4-7.
} flash_security_disable_packet_t;

//! @brief WriteMemory packet format.
typedef struct WriteMemoryPacket
{
    command_packet_t commandPacket; //!< header
    uint32_t startAddress;          //!< Paremeter 0: Start address of memory to write to.
    uint32_t byteCount;             //!< Parameter 1: Number of bytes to write.
} write_memory_packet_t;



void bootloader_data_sink(uint8_t byte);
void bootloader_run(void);
void application_run(void);
bool bootloader_isActive();



#endif // __BL_CORE_H__
