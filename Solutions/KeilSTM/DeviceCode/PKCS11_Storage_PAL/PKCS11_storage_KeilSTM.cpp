////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////#include <Tinyhal.h>
#include "PKCS11_storage_KeilSTM.h"

//--//
BOOL  SecureStorage_KeilSTM::CreateFile( LPCSTR fileName , LPCSTR groupName, UINT32  fileType, UINT8* data, UINT32  dataLength )
{
    return FALSE;
}
BOOL  SecureStorage_KeilSTM::ReadFile( LPCSTR fileName , LPCSTR groupName, UINT32& fileType, UINT8* data, UINT32& dataLength )
{
    return FALSE;
}
BOOL  SecureStorage_KeilSTM::GetFileEnum( LPCSTR groupName, UINT32 fileType , FileEnumCtx& enumCtx )
{
    return FALSE;
}
BOOL  SecureStorage_KeilSTM::GetNextFile( FileEnumCtx& enumCtx, CHAR*fileName,UINT32 fileNameLen )
{
    return FALSE;
}
BOOL  SecureStorage_KeilSTM::Delete( LPCSTR fileName , LPCSTR groupName )
{
    return FALSE;
}

