////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>

#include "../BlockStorage_HAL\BlockStorageDriver_DK_LM3S9B96.h"




// ****** TO BE MODIFIED *****************************
// BlockDeviceInformation
#define FLASH_MANUFACTURER_CODE                 0x4C4D  //" LM "
#define FLASH_DEVICE_CODE                       0x3353  //" 3S "  
#define FLASH_BLOCK_COUNT                       (LM3S_IFLASH_SIZE / LM3S_IFLASH_BLOCK_SIZE)


#define LM3S_BLOCK_ERASE_TYPICAL_TIME_USEC     ( 150ul * 1000)
#define LM3S_SECTOR_WRITE_TYPICAL_TIME_USEC    (  16ul * 1000)
#define LM3S_WORD_WRITE_TYPICAL_TIME_USEC      (            6)
#define LM3S_BLOCK_ERASE_MAX_TIME_USEC         (1000ul * 1000)
#define LM3S_SECTOR_WRITE_MAX_TIME_USEC        (  48ul * 1000)
#define LM3S_WORD_WRITE_MAX_TIME_USEC          (           12)


#define LM3S_BS__BASE_ADDRESS                  LM3S_IFLASH
#define LM3S_BS__SIZE_IN_BYTES                 LM3S_IFLASH_SIZE

//dummy
#define LM3S_BS__CHIP_SELECT                   0
#define LM3S_BS__WAIT_STATES                   0
#define LM3S_BS__RELEASE_COUNTS                0
#define LM3S_BS__BIT_WIDTH                     32
#define LM3S_BS__WP_GPIO_PIN                   GPIO_PIN_NONE
#define LM3S_BS__WP_ACTIVE                     FALSE

#define LM3S_BS__IS_REMOVABLE                  false
#define LM3S_BS__SUPPORTS_XIP                  false
#define LM3S_BS__WRITE_PROTECTED               false

#define LM3S_BS__NUM_REGIONS                   1

#define BYTES_PER_SECTOR                       LM3S_IFLASH_SECTOR_SIZE
    
#define REGION1_XIP_ADDRESS                    LM3S_BS__BASE_ADDRESS
#define REGION1_BYTES_PER_BLK                  LM3S_IFLASH_BLOCK_SIZE
#define REGION1_NUM_OF_BLOCKS                  FLASH_BLOCK_COUNT

//*******************************************************//



//--//


#if defined(MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP)
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP 
#endif

#if defined(MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE)
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE
#endif

#ifdef __GNUC__
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP  BlockRange::BLOCKTYPE_BOOTSTRAP
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE       BlockRange::BLOCKTYPE_CODE
#else
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP  BlockRange::BLOCKTYPE_CODE
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE       BlockRange::BLOCKTYPE_DEPLOYMENT
#endif

//--//


const BlockRange g_LM3S_BS_BlockRange[] =
{
    { BlockRange::BLOCKTYPE_CODE            ,  0, 6},
    { BlockRange::BLOCKTYPE_CONFIG          ,  7, 7},
    { BlockRange::BLOCKTYPE_DEPLOYMENT      ,  8, 9},
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 10, 11},
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 12, 13},
    { BlockRange::BLOCKTYPE_STORAGE_A       , 14, 14},
    { BlockRange::BLOCKTYPE_STORAGE_B       , 15, 15},
};

//--//

#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP 
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE    

//--//

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rwdata = "g_LM3S_BS_Config"
#endif




const BlockRegionInfo  g_LM3S_BS_BlkRegion[LM3S_BS__NUM_REGIONS] = 
{
    
    REGION1_XIP_ADDRESS,                // ByteAddress     Address;            // Start address
    REGION1_NUM_OF_BLOCKS,              // UINT32          NumBlocks;          // total number of blocks in this region
    REGION1_BYTES_PER_BLK,              // UINT32          BytesPerBlock;      // Total number of bytes per block (MUST be SectorsPerBlock * DataBytesPerSector)

    ARRAYSIZE_CONST_EXPR(g_LM3S_BS_BlockRange),
    g_LM3S_BS_BlockRange,
};

//--//


BlockDeviceInfo g_LM3S_BS_DeviceInfo=
{
    {
        LM3S_BS__IS_REMOVABLE,                     // BOOL Removable;
        LM3S_BS__SUPPORTS_XIP,                     // BOOL SupportsXIP;
        LM3S_BS__WRITE_PROTECTED,                  // BOOL WriteProtected
        0,                                         // BOOL SupportsCopyBack;
    },
    
    LM3S_WORD_WRITE_MAX_TIME_USEC,                 // UINT32 Duration_Max_WordWrite_uSec;
    LM3S_BLOCK_ERASE_MAX_TIME_USEC,                // UINT32 Duration_Max_SectorErase_uSec;
    BYTES_PER_SECTOR,                              // BytesPerSector; // Bytes Per Sector

    LM3S_BS__SIZE_IN_BYTES,                        // UINT32 Size;

    LM3S_BS__NUM_REGIONS,                          // UINT32 NumRegions;
    g_LM3S_BS_BlkRegion,                           // const BlockRegionInfo* pRegions;
};


struct MEMORY_MAPPED_NOR_BLOCK_CONFIG g_LM3S_BS_Config =
{
    { // BLOCK_CONFIG
        {
            LM3S_BS__WP_GPIO_PIN,                  // GPIO_PIN             Pin;
            LM3S_BS__WP_ACTIVE,                    // BOOL                 ActiveState;
        },

        &g_LM3S_BS_DeviceInfo,                     // BlockDeviceinfo
    },
    
    { // CPU_MEMORY_CONFIG
        LM3S_BS__CHIP_SELECT,                      // UINT8                CPU_MEMORY_CONFIG::ChipSelect;
        TRUE,                                       // UINT8                CPU_MEMORY_CONFIG::ReadOnly;
        LM3S_BS__WAIT_STATES,                      // UINT32               CPU_MEMORY_CONFIG::WaitStates;
        LM3S_BS__RELEASE_COUNTS,                   // UINT32               CPU_MEMORY_CONFIG::ReleaseCounts;
        LM3S_BS__BIT_WIDTH,                        // UINT32               CPU_MEMORY_CONFIG::BitWidth;
        LM3S_BS__BASE_ADDRESS,                     // UINT32               CPU_MEMORY_CONFIG::BaseAddress;
        LM3S_BS__SIZE_IN_BYTES,                    // UINT32               CPU_MEMORY_CONFIG::SizeInBytes;
        0,                                          // UINT8                CPU_MEMORY_CONFIG::XREADYEnable 
        0,                                          // UINT8                CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                                          // UINT8                CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0x1055AADD,                                     // UINT32               ChipProtection;
    FLASH_MANUFACTURER_CODE,                        // FLASH_WORD           ManufacturerCode;
    FLASH_DEVICE_CODE,                              // FLASH_WORD           DeviceCode;
};


//--//

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_LM3S_BS"
#endif

struct BlockStorageDevice g_LM3S_BS;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 

//--//

