
#include "lwiplib.h"
#include "adapt.h"

//*****************************************************************************
//
// Globals required by SSL library.
//
//*****************************************************************************
sslKeys_t *g_pSslKeys;
extern unsigned long g_ulPrivKeySrvLen;
extern unsigned long g_ulCertSrvLen;
extern unsigned char g_pcCertSrv[];
extern unsigned char g_pcPrivKeySrv[];

//****************************************************************************
//
// Public API functions
//
//****************************************************************************
err_t
ssl_init(void)
{
    int iRet;

    DEBUG_MSG("ssl_init\n");

    //
    // Initialize the SSL library.
    //
    iRet = matrixSslOpen();
    if(iRet < 0)
    {
        //
        // Something went wrong so kill the application with an error code.
        //
        return(ERR_IF);
    }

    //
    // Read the required certificates from memory.
    //
    iRet = matrixSslReadKeysMem(&g_pSslKeys, g_pcCertSrv, g_ulCertSrvLen,
                                g_pcPrivKeySrv, g_ulPrivKeySrvLen, NULL, 0);
    if(iRet < 0)
    {
        //
        // Something went wrong so kill the application with an error code.
        //
        return(ERR_MEM);
    }

    return(ERR_OK);
}