////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#include <tinyhal.h>
#include <tinyhal.h>

//--//
#ifndef _DRIVERS_BLOCKSTORAGEDRIVER_DK_LM3S9B96_H_
#define _DRIVERS_BLOCKSTORAGEDRIVER_DK_LM3S9B96_H_ 1

// Internal FLASH base address
#define LM3S_IFLASH                    (0x60000000)    
// Internal FLASH size in byte (256 Kbytes)
#define LM3S_IFLASH_SIZE               (0x00100000)    
// Internal FLASH Page Size: 256 bytes
#define LM3S_IFLASH_PAGE_SIZE          (256)           
// Internal FLASH Lock Region Size: 16 Kbytes
#define LM3S_IFLASH_LOCK_REGION_SIZE   (64 * 1024)
// Internal FLASH Number of Pages: 2048 bytes
#define LM3S_IFLASH_NB_OF_PAGES        (LM3S_IFLASH_SIZE/LM3S_IFLASH_PAGE_SIZE)          
// Internal FLASH Number of Lock Bits: 32 bytes
#define LM3S_IFLASH_NB_OF_LOCK_BITS    (32)            
#define LM3S_IFLASH_SECTOR_SIZE        (4 * 1024)
//#define LM3S_IFLASH_SECTOR_PER_PAGE    (LM3S_IFLASH_PAGE_SIZE/LM3S_IFLASH_SECTOR_SIZE)
#define LM3S_IFLASH_BLOCK_SIZE         (64 * 1024)
#define LM3S_IFLASH_PAGE_PER_BLOCK     (LM3S_IFLASH_BLOCK_SIZE/LM3S_IFLASH_PAGE_SIZE)
#define LM3S_IFLASH_SECTOR_PER_BLOCK   (LM3S_IFLASH_BLOCK_SIZE/LM3S_IFLASH_SECTOR_SIZE)

//--//

struct DK_LM3S9B96_BlockStorage_Driver
{
    typedef UINT32 CHIP_WORD;
    //--//

    static BOOL ChipInitialize( void* context );

    static BOOL ChipUnInitialize( void* context );

    static const BlockDeviceInfo* GetDeviceInfo( void* context );

    static BOOL Read( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff);
    
    static BOOL Write( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite);

    static BOOL Memset( void* context, ByteAddress Address, UINT8 Data, UINT32 NumBytes );

    static BOOL GetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

    static BOOL SetSectorMetadata(void* context, ByteAddress SectorStart, SectorMetadata* pSectorMetadata);

    static BOOL IsBlockErased( void* context, ByteAddress Address, UINT32 BlockLength );

    static BOOL EraseBlock( void* context, ByteAddress Address );

    static void SetPowerState( void* context, UINT32 State );

    static UINT32 MaxSectorWrite_uSec(void* context);

    static UINT32 MaxBlockErase_uSec(void* context);

    static BOOL ChipReadOnly( void* context, BOOL On, UINT32 ProtectionKey );

    static BOOL ReadProductID( void* context, FLASH_WORD& ManufacturerCode, FLASH_WORD& DeviceCode );
    
private:
    static BOOL WriteX( void* context, ByteAddress Address, UINT32 NumBytes, BYTE * pSectorBuff, BOOL ReadModifyWrite, BOOL fIncrementDataPtr );
};

//--//

#endif // _DRIVERS_BLOCKSTORAGEDRIVER_DK_LM3S9B96_H_

