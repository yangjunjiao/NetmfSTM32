#include "ssl_functions.h"

sslKeys_t *g_pSslKeys;
extern unsigned long g_ulPrivKeySrvLen;
extern unsigned long g_ulCertSrvLen;
extern unsigned char g_pcCertSrv[];
extern unsigned char g_pcPrivKeySrv[];
extern unsigned char g_pcAcertSrv[];
extern unsigned long g_ulAcertSrvLen;

#include <stdlib.h>
#include <stdio.h>

BOOL ssl_initialize_internal() {

	int iRet;
	//
	// Initialize the SSL library.
	//
	iRet = matrixSslOpen();
	if (iRet < 0) {
		//
		// Something went wrong so kill the application with an error code.
		//
		MATRIXSSL_PERROR("MatrixSSL library open failure.  Exiting\n");
		return (FALSE);
	}

	iRet = matrixSslNewKeys(&g_pSslKeys);

	if (iRet < 0) {
		MATRIXSSL_PERROR("MatrixSSL library key init failure.  Exiting\n");
		matrixSslDeleteKeys(g_pSslKeys);
		matrixSslClose();
		//
		// Something went wrong so kill the application with an error code.
		//
		return (FALSE);
	}

	//
	// Read the required certificates from memory.
	//
	iRet = matrixSslLoadRsaKeysMem(g_pSslKeys, g_pcCertSrv, g_ulCertSrvLen,
			g_pcPrivKeySrv, g_ulPrivKeySrvLen, g_pcAcertSrv, g_ulAcertSrvLen);

	//TODO or generate it
	if (iRet < 0) {
		MATRIXSSL_PERROR("No certificate material loaded.  Exiting\n");
		matrixSslDeleteKeys(g_pSslKeys);
		matrixSslClose();
		//
		// Something went wrong so kill the application with an error code.
		//
		return (FALSE);
	}

	MATRIXSSL_PDEBUG("OK\n");
	return TRUE;
}

