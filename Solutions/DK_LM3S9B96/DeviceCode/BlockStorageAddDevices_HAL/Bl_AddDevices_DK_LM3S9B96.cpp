////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>

//--//

extern struct BlockStorageDevice  g_LM3S_BS;
extern struct IBlockStorageDevice g_LM3S_BS_DeviceTable;
extern struct BLOCK_CONFIG        g_LM3S_BS_Config; 

void BlockStorage_AddDevices()
{   
    BlockStorageList::AddDevice( &g_LM3S_BS, &g_LM3S_BS_DeviceTable, &g_LM3S_BS_Config, FALSE );
}
