#include "ssl_functions.h"


sslKeys_t *g_pSslKeys;
extern unsigned long g_ulPrivKeySrvLen;
extern unsigned long g_ulCertSrvLen;
extern unsigned char g_pcCertSrv[];
extern unsigned char g_pcPrivKeySrv[];
#include <stdlib.h>
#include <stdio.h>

BOOL ssl_initialize_internal()
{
    

	 int iRet;

    debug_printf("ssl_init\n");

    //
    // Initialize the SSL library.
    //
	
    iRet = matrixSslOpen();
    if(iRet < 0)
    {
        //
        // Something went wrong so kill the application with an error code.
        //
        return(FALSE);
    }

	
    //
    // Read the required certificates from memory.
    //
    iRet = matrixSslLoadRsaKeysMem(g_pSslKeys, g_pcCertSrv, g_ulCertSrvLen,
                                g_pcPrivKeySrv, g_ulPrivKeySrvLen, NULL, 0);
    if(iRet < 0)
    {
    	TINYCLR_SSL_PRINTF("No certificate material loaded.  Exiting\n");
    	matrixSslDeleteKeys(g_pSslKeys);
    	matrixSslClose();
        //
        // Something went wrong so kill the application with an error code.
        //
        return(FALSE);
    }
	
	
    return TRUE;
}

