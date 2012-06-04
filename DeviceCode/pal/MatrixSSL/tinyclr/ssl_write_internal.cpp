#include "ssl_functions.h"
#include "adapt/adapt.h"

static const int c_MaxSslDataSize = 1460;

int ssl_write_internal(int socket, const char* data, size_t size) {
	unsigned char *sslData;
	int rc = 0;
	SSL* ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		return SOCK_SOCKET_ERROR;
	}

	MATRIXSSL_PDEBUG_ALL("------------- called ----------------: Socket: %i\n", socket);
/*
	SSL* ssl = (SSL*) g_SSL_Driver.m_sslContextArray[0].SslContext;
	if (ssl == NULL) {
		return SOCK_SOCKET_ERROR;
	}*/

	int available = 0, requested = size, sent = 0;
	if ((available = matrixSslGetWritebuf(ssl, &sslData, size)) < 0) {
		return SOCK_SOCKET_ERROR;
	}
	requested = min(requested, available);
	TINYCLR_SSL_MEMCPY(sslData, data, requested);
	rc = matrixSslEncodeWritebuf(ssl, requested);
	//PRINT_RETURN_VALUE(rc); //if > 0 number of bytes encoded
	if (rc < 0) {
		PRINT_UNEXPECTED_RETURN_VALUE(rc);
		return SOCK_SOCKET_ERROR;
	}
	int len = matrixSslGetOutdata(ssl, &sslData);

	sent = SOCK_send(socket, (const char *) sslData, len, 0);
	rc = matrixSslSentData(ssl, sent);
	PRINT_RETURN_VALUE(rc);
	if (rc == MATRIXSSL_SUCCESS) {
		return sent;
	}
	PRINT_UNEXPECTED_RETURN_VALUE(rc);
	return SOCK_SOCKET_ERROR;
}

