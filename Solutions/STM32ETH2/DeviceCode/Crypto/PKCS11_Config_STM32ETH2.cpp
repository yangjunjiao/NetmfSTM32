#include <PAL\PKCS11\CryptokiPAL.h>

//extern CK_SLOT_INFO  g_OpenSSL_SlotInfo;
//extern CryptokiToken g_OpenSSL_Token;


 CK_SLOT_INFO  g_Dummy_SlotInfo;
 CryptokiToken g_Dummy_Token;

CryptokiSlot  g_CryptokiSlots[] = 
{
    {
        &g_Dummy_SlotInfo,
        &g_Dummy_Token,
    },
};

const UINT32 g_CryptokiSlotCount = ARRAYSIZE(g_CryptokiSlots);


