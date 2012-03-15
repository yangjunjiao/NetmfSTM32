////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "BlockStorageDriver_DK_LM3S9B96.h"

//Include Stellaris H/W Header files
#include "ssiflash.h"


UINT8 pageBuffer[LM3S_IFLASH_PAGE_SIZE];
//--//

#if defined(arm) || defined(__arm)
#pragma arm section code = "SectionForFlashOperations"
#endif

BOOL DK_LM3S9B96_BlockStorage_Driver::ChipInitialize( void* context )
{
    return (BOOL)SSIFlashInit();
}

BOOL DK_LM3S9B96_BlockStorage_Driver::ChipUnInitialize( void* context )
{
    
    return TRUE;
}

const BlockDeviceInfo* DK_LM3S9B96_BlockStorage_Driver::GetDeviceInfo( void* context )
{
    BLOCK_CONFIG* config = (BLOCK_CONFIG *)context;
    return config->BlockDeviceInformation;
}

BOOL  DK_LM3S9B96_BlockStorage_Driver::ChipReadOnly( void* context, BOOL On, UINT32 ProtectionKey )
{
    return TRUE;
}


BOOL  DK_LM3S9B96_BlockStorage_Driver::Read( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff)
{
    unsigned long chipAddress = (unsigned long) Address;
    unsigned long endAddress  = (unsigned long) Address + NumBytes;
    unsigned char *pBuf       = (unsigned char *)pSectorBuff;
    
    //Check valid address
    if( !( (chipAddress >= LM3S_IFLASH ) && (endAddress < (LM3S_IFLASH + LM3S_IFLASH_SIZE)) ) ) return FALSE;
    
    chipAddress -= LM3S_IFLASH;
    endAddress  -= LM3S_IFLASH;
    
    if( SSIFlashRead(chipAddress, (unsigned long) NumBytes, pBuf) == NumBytes)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}    



BOOL DK_LM3S9B96_BlockStorage_Driver::Write(void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite)
{
    return WriteX( context, Address, NumBytes, pSectorBuff, ReadModifyWrite, TRUE );
}

BOOL DK_LM3S9B96_BlockStorage_Driver::WriteX( void* context, ByteAddress address, UINT32 numBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite, BOOL fIncrementDataPtr )
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *    deviceInfo = config->BlockConfig.BlockDeviceInformation;

    BYTE*  localpData = pSectorBuff;
    
    UINT32 baseAddress;
    UINT32 endAddress;
    UINT32 pageNumber;
    UINT32 offset;
    UINT32 writeSize;
    UINT32 pageAddress;
    UINT32 iRegion, iRange;
    INT32  NumOfBytes = (INT32)numBytes;
    
    CHIP_WORD *chipAddress;
        
    if (deviceInfo->Attribute.WriteProtected) return FALSE;

    if (!deviceInfo->FindRegionFromAddress(address, iRegion, iRange)) return FALSE;

    address     = address;
    baseAddress = deviceInfo->Regions[iRegion].Start;
    endAddress  = baseAddress + deviceInfo->Regions[iRegion].Size();
    chipAddress = (CHIP_WORD *)address;
    
    if(( address < baseAddress) || (address >= endAddress)) return FALSE;

    pageNumber  = ((address - baseAddress) / LM3S_IFLASH_PAGE_SIZE);
    offset      = ((address - baseAddress) % LM3S_IFLASH_PAGE_SIZE);
    pageAddress = pageNumber * LM3S_IFLASH_PAGE_SIZE + baseAddress;

    ChipReadOnly( context, FALSE, FLASH_PROTECTION_KEY );

    while(NumOfBytes > 0)
    {
        unsigned char* pSrcBuf;
        
        writeSize = __min(LM3S_IFLASH_PAGE_SIZE - offset, NumOfBytes);
        
        
        /* //Blank Check before write data
        CHIP_WORD * tmpWrData =(CHIP_WORD*) localpData;
        for (UINT32 i=0; i <writeSize; i+=sizeof(CHIP_WORD))
        {
            CHIP_WORD Data;
            Data = *tmpWrData;
            if(0 != (Data & ~(*chipAddress)))
            {
                debug_printf( "Erase failure: 0x%08x=0x%08x, writeTo(0x%08x)\r\n", (size_t)chipAddress, *chipAddress, Data );
                ASSERT(0);
                return FALSE;
            }
            chipAddress ++;
            tmpWrData ++;

        }
        */
        
        // get the whole page first, process boundry save write
        if(offset)
        {
            SSIFlashRead(pageAddress - baseAddress, LM3S_IFLASH_PAGE_SIZE, pageBuffer);
            memcpy(pageBuffer + offset, localpData, writeSize);
            pSrcBuf = pageBuffer;
        }
        else if( writeSize != LM3S_IFLASH_PAGE_SIZE)
        {
            SSIFlashRead(pageAddress - baseAddress, LM3S_IFLASH_PAGE_SIZE, pageBuffer);
            memcpy(pageBuffer, localpData, writeSize);
            pSrcBuf = pageBuffer;
        }
        else
        {
            pSrcBuf = localpData;
        }
        
        // --//
        // Set EFC Mode Register - number of cycles during 1.5 microsecond
        {
            GLOBAL_LOCK(irq);

            SSIFlashWrite(pageAddress-baseAddress,LM3S_IFLASH_PAGE_SIZE, pSrcBuf);
        }
        NumOfBytes -= writeSize;
        if(fIncrementDataPtr) localpData += writeSize;
        pageAddress += LM3S_IFLASH_PAGE_SIZE;
        pageNumber++;
        offset = 0;
    }

    ChipReadOnly( context, TRUE, FLASH_PROTECTION_KEY );
    return TRUE;

}

BOOL DK_LM3S9B96_BlockStorage_Driver::Memset(void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes)
{
    CHIP_WORD chipData;

    memset(&chipData, Data, sizeof(CHIP_WORD));
    
    return WriteX(context, Address, NumBytes, (BYTE*)&chipData, TRUE, FALSE);
}

BOOL DK_LM3S9B96_BlockStorage_Driver::GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    return FALSE;
}

BOOL DK_LM3S9B96_BlockStorage_Driver::SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata)
{
    return FALSE;
}



BOOL DK_LM3S9B96_BlockStorage_Driver::IsBlockErased( void* context, ByteAddress Address, UINT32 BlockLength )
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *     deviceInfo = config->BlockConfig.BlockDeviceInformation;

    UINT32 iRegion, iRange;
    unsigned long StartAddress, EndAddress, PageAddress;
    

    // 1st or 2nd FLASH controller 
    if (!deviceInfo->FindRegionFromAddress( Address, iRegion, iRange )) return FALSE;

    Address = CPU_GetCachableAddress(Address);

    StartAddress = (unsigned long) Address;
    EndAddress  =  (unsigned long)(Address + BlockLength);
    
    for(PageAddress = StartAddress; PageAddress < EndAddress; PageAddress+=LM3S_IFLASH_PAGE_SIZE)
    {
        unsigned long ulAddress = PageAddress - LM3S_IFLASH;
        
        if( SSIFlashRead(ulAddress,LM3S_IFLASH_PAGE_SIZE,pageBuffer) == LM3S_IFLASH_PAGE_SIZE)
        {
            int i, LoopCnt;
            UINT32 *pData = (UINT32*)pageBuffer;
            
            for(i=0, LoopCnt = (LM3S_IFLASH_PAGE_SIZE>>2); i< LoopCnt; i++)
            {
                if( *pData++ != 0xFFFFFFFF)
                {
                    return FALSE;
                }
            }
        }
        else
        {
            return FALSE;
        }
    }
    
    return TRUE;
}


BOOL DK_LM3S9B96_BlockStorage_Driver::EraseBlock( void* context, ByteAddress address )
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    const BlockDeviceInfo *     deviceInfo = config->BlockConfig.BlockDeviceInformation;

    UINT32 iRegion, iRange;
    UINT32 baseAddress, BlockAddress;
    BOOL   fBlockEraseOk = TRUE;
   
    // 1st or 2nd FLASH controller 
    if (!deviceInfo->FindRegionFromAddress( address, iRegion, iRange )) return FALSE;

    address     = address;
    baseAddress = deviceInfo->Regions[iRegion].Start;

    BlockAddress= address - baseAddress;

    ChipReadOnly( context, FALSE, FLASH_PROTECTION_KEY );
    
    {
        GLOBAL_LOCK(irq);
        
        if( SSIFlashBlockErase(BlockAddress, true) )
        {
            fBlockEraseOk = TRUE;
        }
        else
        {
            fBlockEraseOk = FALSE;
        }
    }

    ChipReadOnly( context, TRUE, FLASH_PROTECTION_KEY );

    return fBlockEraseOk;
}

void  DK_LM3S9B96_BlockStorage_Driver::SetPowerState( void* context, UINT32 State )
{
    return;
}

//--//

// Public functions

BOOL DK_LM3S9B96_BlockStorage_Driver::ReadProductID( void* context, FLASH_WORD& ManufacturerCode, FLASH_WORD& DeviceCode )
{
    unsigned char ucManufacturer, ucDevice;
    
    if( SSIFlashIDGet(&ucManufacturer,&ucDevice) )
    {
        ManufacturerCode = (FLASH_WORD) ucManufacturer;
        DeviceCode       = (FLASH_WORD) ucDevice;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}



//--// ---------------------------------------------------

#if defined(arm) || defined(__arm)
#pragma arm section code = "SectionForFlashOperations"
#endif

UINT32 DK_LM3S9B96_BlockStorage_Driver::MaxSectorWrite_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;

    return config->BlockConfig.BlockDeviceInformation->MaxSectorWrite_uSec;
}


UINT32 DK_LM3S9B96_BlockStorage_Driver::MaxBlockErase_uSec( void* context )
{
    NATIVE_PROFILE_PAL_FLASH();
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;

    return config->BlockConfig.BlockDeviceInformation->MaxBlockErase_uSec;
}

#if defined(arm) || defined(__arm)
#pragma arm section code 
#endif


//--// 
/*
#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata = "g_LM3S_BS_DeviceTable"
#endif
*/

struct IBlockStorageDevice g_LM3S_BS_DeviceTable = 
{                          
    &DK_LM3S9B96_BlockStorage_Driver::ChipInitialize,
    &DK_LM3S9B96_BlockStorage_Driver::ChipUnInitialize,
    &DK_LM3S9B96_BlockStorage_Driver::GetDeviceInfo,
    &DK_LM3S9B96_BlockStorage_Driver::Read,
    &DK_LM3S9B96_BlockStorage_Driver::Write,
    &DK_LM3S9B96_BlockStorage_Driver::Memset,
    &DK_LM3S9B96_BlockStorage_Driver::GetSectorMetadata,
    &DK_LM3S9B96_BlockStorage_Driver::SetSectorMetadata,
    &DK_LM3S9B96_BlockStorage_Driver::IsBlockErased,
    &DK_LM3S9B96_BlockStorage_Driver::EraseBlock,
    &DK_LM3S9B96_BlockStorage_Driver::SetPowerState,
    &DK_LM3S9B96_BlockStorage_Driver::MaxSectorWrite_uSec,
    &DK_LM3S9B96_BlockStorage_Driver::MaxBlockErase_uSec,    
};
/*
#if defined(ADS_LINKER_BUG__NOT_ALL_UNUSED_VARIABLES_ARE_REMOVED)
#pragma arm section rodata 
#endif 
*/


