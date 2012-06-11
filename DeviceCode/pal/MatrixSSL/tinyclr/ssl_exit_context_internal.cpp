#include "ssl_functions.h"
#include "adapt/adapt.h"


//TODO check if called in server mode
BOOL ssl_exit_context_internal(int sslContextHandle )
{
	MATRIXSSL_PDEBUG("sslContext: %i\n", sslContextHandle);

	SSL *ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySslIndex(sslContextHandle);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
			ssl = (SSL*) sslContext->SslContext;
		} else {
			MATRIXSSL_PERROR("Context not valid\n");
			return FALSE;
		}

    matrixSslDeleteSession(ssl);
    g_SSL_Driver.DeleteSslContext(sslContextHandle);
    

    return TRUE;
}


