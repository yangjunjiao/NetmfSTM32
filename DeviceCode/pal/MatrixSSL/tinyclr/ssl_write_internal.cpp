#include "ssl_functions.h"
#include "adapt/adapt.h"

static const int c_MaxSslDataSize = 1460;

int ssl_write_internal(int sd, const char* data, size_t size) {
	unsigned char *sslData;

	SSL* ssl = (SSL*) g_SSL_Driver.m_sslContextArray[0].SslContext;
	if (ssl == NULL) {
		return SOCK_SOCKET_ERROR;
	}

	int available = 0, requested = size, sent = 0;
	if ((available = matrixSslGetWritebuf(ssl, &sslData, size)) < 0) {
		return SOCK_SOCKET_ERROR;
	}
	requested = min(requested, available);
	TINYCLR_SSL_MEMCPY(sslData, data, requested);
	if (matrixSslEncodeWritebuf(ssl, requested) < 0) {
		return SOCK_SOCKET_ERROR;
	}
	int len = matrixSslGetOutdata(ssl, &sslData);

	sent = SOCK_send(sd, (const char *) sslData, len, 0);
	int rc = matrixSslSentData(ssl, sent);
	if (rc == MATRIXSSL_SUCCESS) {
		return sent;
	}

	return SOCK_SOCKET_ERROR;
}

