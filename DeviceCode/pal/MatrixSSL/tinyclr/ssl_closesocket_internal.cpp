#include "ssl_functions.h"
#include "adapt/adapt.h"

int ssl_sendData(SSL_Conext* sslContext) {
	unsigned char *sslData;
	int len = matrixSslGetOutdata(sslContext->SslContext, &sslData);
	int sent = SOCK_send(sslContext->SocketHandle, (const char *) sslData, len,
			0);
	if(sent ==  SOCK_SOCKET_ERROR)
	{
		MATRIXSSL_PERROR("SOCK_SOCKET_ERROR\n");
		return SOCK_SOCKET_ERROR;
	}
	int rc = matrixSslSentData(sslContext->SslContext, sent);
	PRINT_RETURN_VALUE(rc);
	if (rc == MATRIXSSL_REQUEST_SEND) {
		return ssl_sendData(sslContext);
	}
	return rc;
}

int ssl_closesocket_internal(int socket) {

	SSL* ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		return SOCK_SOCKET_ERROR;
	}

	int rc = matrixSslEncodeClosureAlert(ssl);
	if (rc != MATRIXSSL_SUCCESS) {
		MATRIXSSL_PERROR(
				"matrixSslEncodeClosureAlert not created, result: %i\n", rc);
		return SOCK_SOCKET_ERROR;
	}

	rc = ssl_sendData(sslContext);
	if (rc != MATRIXSSL_REQUEST_CLOSE) {
		MATRIXSSL_PERROR("socket close error\n");
		return SOCK_SOCKET_ERROR;
	}

	MATRIXSSL_PDEBUG("Closing socket\n")
	return SOCK_close(socket);
}

