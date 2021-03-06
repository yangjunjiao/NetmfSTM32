//-----------------------------------------------------------------------------
// Software that is described herein is for illustrative purposes only  
// which provides customers with programming information regarding the  
// products. This software is supplied "AS IS" without any warranties.  
// NXP Semiconductors assumes no responsibility or liability for the 
// use of the software, conveys no license or title under any patent, 
// copyright, or mask work right to the product. NXP Semiconductors 
// reserves the right to make changes in the software without 
// notification. NXP Semiconductors also make no representation or 
// warranty that such application will be suitable for the specified 
// use without further testing or modification. 
//-----------------------------------------------------------------------------

#include <tinyhal.h>
#include <Drivers\BlockStorage\Flash\AM29DL_16\AM29DL_16.h>


//--//

#define FLASH_MANUFACTURER_CODE                0x0001
//#define FLASH_DEVICE_CODE                       0x227E
#define FLASH_DEVICE_CODE                       0x220C
#define FLASH_BLOCK_COUNT                       135
#define FLASH_BLOCK_ERASE_TYPICAL_TIME_USEC   700000
#define FLASH_SECTOR_WRITE_TYPICAL_TIME_USEC  210
#define FLASH_BLOCK_ERASE_MAX_TIME_USEC       12000000
#define FLASH_SECTOR_WRITE_MAX_TIME_USEC      220

//--//

#define S29GL064A_16__CHIP_SELECT      0
#define S29GL064A_16__WAIT_STATES      0 
#define S29GL064A_16__RELEASE_COUNTS   0
#define S29GL064A_16__BIT_WIDTH        32 
#define S29GL064A_16__BASE_ADDRESS     FLASH_MEMORY_Base
#define S29GL064A_16__SIZE_IN_BYTES    FLASH_MEMORY_Size
#define S29GL064A_16__WP_GPIO_PIN      GPIO_PIN_NONE
#define S29GL064A_16__WP_ACTIVE        FALSE


#if defined(MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP)
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP
#endif

#if defined(MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE)
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE
#endif

#ifdef __GNUC__
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP BlockRange::BLOCKTYPE_BOOTSTRAP
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE      BlockRange::BLOCKTYPE_CODE
#else
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP BlockRange::BLOCKTYPE_CODE
#define MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE      BlockRange::BLOCKTYPE_DEPLOYMENT
#endif


// -- // device information

// BlockDeviceInformation

#define S29GL064A_16__IS_REMOVABLE        FALSE
#define S29GL064A_16__SUPPORTS_XIP        TRUE
#define S29GL064A_16__WRITE_PROTECTED     FALSE

//#define S29GL064A_16__NUM_REGIONS         2 //Flash memory : S29GL064A90TAI03
#define S29GL064A_16__NUM_REGIONS         1  //Flash memory : S29GL064N90TFI01




//--//
/* 
//Flash memory : S29GL064A90TAI03
const BlockRange g_S29GL064A_16_BlockRange1[] =
{
    { BlockRange::BLOCKTYPE_BOOTSTRAP       ,  0,  0 },       // SA0    - Bootloader - Reserve 128K for Compressed Flashlite/TinyBooter (0-7FFF)
    { BlockRange::BLOCKTYPE_CODE            ,  1, 35 },       // SA1-35 - ER_FLASH (10000-23FFFF)
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 36, 47 },       // SA36-47 - DEPLOYMENT(240000-2FFFFF)
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 48, 59 },       // SA48-59 - DEPLOYMENT(300000-3BFFFF)
    { BlockRange::BLOCKTYPE_FILESYSTEM      , 60, 126 },      // SA60-123 - FILESYSTEM(3C0000-7EFFFF)  

};

const BlockRange g_S29GL064A_16_BlockRange2[] =
{
    { BlockRange::BLOCKTYPE_CONFIG          , 0,  1 },       // SA127-128 - ER_Config(7F0000-7F3FFF)
    { BlockRange::BLOCKTYPE_STORAGE_A       , 2,  4 },       // SA129-131 - Storage A(7F4000-7F9FFF)
//    { BlockRange::BLOCKTYPE_STORAGE_B       , 5,  7 },       // SA132-134 - Storage B(7FA000-7FFFFF)
    { BlockRange::BLOCKTYPE_STORAGE_B       , 5,  6 },       // SA132-133 - Storage B(7FA000-7FDFFF)
    { BlockRange::BLOCKTYPE_RESERVED         , 7,  7 },       // SA134(7FE000):MAC address definition area
};
*/
//Flash memory : S29GL064N90TFI01
const BlockRange g_S29GL064A_16_BlockRange1[] =
{
    { BlockRange::BLOCKTYPE_BOOTSTRAP       ,  0,  0 },       // SA0    - Bootloader - Reserve 128K for Compressed Flashlite/TinyBooter (0-7FFF)
    { BlockRange::BLOCKTYPE_CODE            ,  1, 35 },       // SA1-35 - ER_FLASH (10000-23FFFF)
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 36, 47 },       // SA36-47 - DEPLOYMENT(240000-2FFFFF)
    { BlockRange::BLOCKTYPE_DEPLOYMENT      , 48, 59 },       // SA48-59 - DEPLOYMENT(300000-3BFFFF)
    { BlockRange::BLOCKTYPE_FILESYSTEM      , 60, 123 },      // SA60-123 - FILESYSTEM(3C0000-7BFFFF) 
    { BlockRange::BLOCKTYPE_CONFIG          , 124,  124 },       // SA124-124 - ER_Config(7C0000-7CFFFF)
    { BlockRange::BLOCKTYPE_STORAGE_A       , 125,  125 },       // SA125-125 - Storage A(7D0000-7DFFFF)
    { BlockRange::BLOCKTYPE_STORAGE_B       , 126,  126 },       // SA126-126 - Storage B(7E0000-7EFFFF)
    { BlockRange::BLOCKTYPE_RESERVED         , 127,  127 },       // SA127-127 - MAC address definition area(7F0000-7FFFFF)

};
//--//

#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_BOOTSTRAP
#undef MEMORY_BLOCKTYPE_GCC_SPECIAL_CODE

//--//

#define BYTES_PER_SECTOR            2
    
#define REGION1_XIP_ADDRESS         S29GL064A_16__BASE_ADDRESS
#define REGION1_BYTES_PER_BLK       0x10000
#define REGION1_SECTORS_PER_BLOCK   REGION1_BYTES_PER_BLK/BYTES_PER_SECTOR
//#define REGION1_NUM_OF_BLOCKS       127  //Flash memory : S29GL064A90TAI03
#define REGION1_NUM_OF_BLOCKS       128  //Flash memory : S29GL064N90TFI01
#define REGION1_SECTOR_START        0

#define REGION2_XIP_ADDRESS         REGION1_XIP_ADDRESS + REGION1_NUM_OF_BLOCKS* REGION1_BYTES_PER_BLK
#define REGION2_BYTES_PER_BLK       0x2000
#define REGION2_SECTORS_PER_BLOCK   REGION2_BYTES_PER_BLK/BYTES_PER_SECTOR
#define REGION2_NUM_OF_BLOCKS       8
#define REGION2_SECTOR_START        REGION1_SECTOR_START + REGION1_NUM_OF_BLOCKS*REGION1_SECTORS_PER_BLOCK



const BlockRegionInfo  g_S29GL064A_16_BlkRegion[S29GL064A_16__NUM_REGIONS] = 
{
    {
        REGION1_XIP_ADDRESS,            // ByteAddress  Start;              // Starting Sector address
        REGION1_NUM_OF_BLOCKS,          // UINT32       NumBlocks;          // total number of blocks in this region
        REGION1_BYTES_PER_BLK,          // UINT32       BytesPerBlock;      // Total number of bytes per block (MUST be SectorsPerBlock * DataBytesPerSector)

        ARRAYSIZE_CONST_EXPR(g_S29GL064A_16_BlockRange1),
        g_S29GL064A_16_BlockRange1
    },
    
/* 
//Flash memory : S29GL064A90TAI03
{
        REGION2_XIP_ADDRESS,            // SectorAddress   Start;              // Starting Sector address
        REGION2_NUM_OF_BLOCKS,          // UINT32          NumBlocks;          // total number of blocks in this region
        REGION2_BYTES_PER_BLK,          // UINT32          BytesPerBlock;      // Total number of bytes per block (MUST be SectorsPerBlock * DataBytesPerSector)

        ARRAYSIZE_CONST_EXPR(g_S29GL064A_16_BlockRange2),
        g_S29GL064A_16_BlockRange2
    },
*/
};

//--//


BlockDeviceInfo g_S29GL064A_16_DeviceInfo=
{
    {
        S29GL064A_16__IS_REMOVABLE,          // BOOL Removable;
        S29GL064A_16__SUPPORTS_XIP,          // BOOL SupportsXIP;
        S29GL064A_16__WRITE_PROTECTED        // BOOL WRiteProtected
    },
    FLASH_SECTOR_WRITE_MAX_TIME_USEC,        // UINT32 Duration_Max_WordWrite_uSec;
    FLASH_BLOCK_ERASE_MAX_TIME_USEC,         // UINT32 Duration_Max_SectorErase_uSec;
    BYTES_PER_SECTOR,                        // UINT32 BytesPerSector; // Bytes Per Sector

    FLASH_MEMORY_Size,                       // UINT32 Size;

    S29GL064A_16__NUM_REGIONS,               // UINT32 NumRegions;
    g_S29GL064A_16_BlkRegion,            // const BlockRegionInfo* pRegions;
};


struct MEMORY_MAPPED_NOR_BLOCK_CONFIG g_S29GL064A_16_BS_Config =
{
    { // BLOCK_CONFIG
        {
            S29GL064A_16__WP_GPIO_PIN,      // GPIO_PIN             Pin;
            S29GL064A_16__WP_ACTIVE,        // BOOL                 ActiveState;
        },

        &g_S29GL064A_16_DeviceInfo,    // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        S29GL064A_16__CHIP_SELECT,          // UINT8                CPU_MEMORY_CONFIG::ChipSelect;
        TRUE,                               // UINT8                CPU_MEMORY_CONFIG::ReadOnly;
        S29GL064A_16__WAIT_STATES,          // UINT32               CPU_MEMORY_CONFIG::WaitStates;
        S29GL064A_16__RELEASE_COUNTS,       // UINT32               CPU_MEMORY_CONFIG::ReleaseCounts;
        S29GL064A_16__BIT_WIDTH,            // UINT32               CPU_MEMORY_CONFIG::BitWidth;
        S29GL064A_16__BASE_ADDRESS,         // UINT32               CPU_MEMORY_CONFIG::BaseAddress;
        S29GL064A_16__SIZE_IN_BYTES,        // UINT32               CPU_MEMORY_CONFIG::SizeInBytes;
        0,                                  // UINT8                CPU_MEMORY_CONFIG::XREADYEnable 
        0,                                  // UINT8                CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                                  // UINT8                CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                                      // UINT32               ChipProtection;
    FLASH_MANUFACTURER_CODE,                // FLASH_WORD           ManufacturerCode;
    FLASH_DEVICE_CODE,                      // FLASH_WORD           DeviceCode;
};

//--//

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_S29GL064A_16_BS"
#endif

struct BlockStorageDevice g_S29GL064A_16_BS;

#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 

//--//

