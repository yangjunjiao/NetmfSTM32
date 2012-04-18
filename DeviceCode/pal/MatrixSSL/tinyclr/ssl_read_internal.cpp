#include "ssl_functions.h"
#include "adapt/adapt.h"

//extern Sockets_LWIP_Driver g_Sockets_LWIP_Driver;

/*
 void printAsString(char* data, len)
 {
 char buf = TINYCLR_SSL_MALLOC(len);
 TINYCLR_SSL_MEMCPY(sslData, data, cpylen);

 }*/
unsigned char* ssl_oldData = NULL;
int ssl_oldDataLength = 0;
unsigned char* ssl_oldDataBase = NULL;
int ssl_oldDataLengthBase = 0;

int ssl_read_internal(int socket, char* data, size_t size) {

	debug_printf("ssl_read_internal() size: %i\n", size);
	unsigned char *sslData;
	SSL* ssl = (SSL*) g_SSL_Driver.m_sslContextArray[0].SslContext;

	if (ssl_oldDataLength != 0) {
		debug_printf("ssl_read_internal() oldData: %i\n", ssl_oldDataLength);
		int cpylen = min(size, ssl_oldDataLength);
		TINYCLR_SSL_MEMCPY(data, ssl_oldData, cpylen);
		if (cpylen == ssl_oldDataLength) {
			//everything copied
			ssl_oldDataLength = 0;
			ssl_oldData = NULL;
			matrixSslProcessedData(ssl, &ssl_oldDataBase, (uint32*) &ssl_oldDataLengthBase);
			debug_printf("ssl_read_internal() oldData processed: %i\n", cpylen);
		} else {
			ssl_oldData = &ssl_oldData[cpylen];
			ssl_oldDataLength = ssl_oldDataLength - cpylen;
		}
		debug_printf("ssl_read_internal() oldData returned: %i\n", cpylen);
		if(cpylen != 0)
			return cpylen;
	}
	if (ssl == NULL) {
		return SOCK_SOCKET_ERROR;
	}

	int len = 0;
	int rc;
	len = matrixSslGetReadbuf(ssl, &sslData);
	len = min (len, size);
	if(len < 10) len = 100;
	int rec = SOCK_recv(socket, (char*) sslData, len, 0);
	debug_printf("ssl_read_internal() SOCK_recv: %i\n", rec);
	if (rec == 0) {
		return rec;
	}
	if ((rc = matrixSslReceivedData(ssl, (int32) rec, &sslData, (uint32*) &len))
			< 0) {
		return SOCK_SOCKET_ERROR;
	}
	debug_printf("ssl_read_internal() matrixSslReceivedData: %i\n", len);
	if (!(rc == MATRIXSSL_APP_DATA || rc == MATRIXSSL_REQUEST_RECV
			|| rc == MATRIXSSL_REQUEST_SEND)) {
		return SOCK_SOCKET_ERROR;
	}
	//g_Sockets_LWIP_Driver.ClearStatusBitsForSocket( socket, FALSE );
	int cpylen = min (size, len);
	if (rc == MATRIXSSL_REQUEST_SEND) {
		int len = matrixSslGetOutdata(ssl, &sslData);

		int sent = SOCK_send(socket, (const char *) sslData, len, 0);
		int rc = matrixSslSentData(ssl, sent);
		if (rc == MATRIXSSL_SUCCESS) {
			return 0;
		}
		return 0;
	}
	if (rc == MATRIXSSL_APP_DATA) {
		TINYCLR_SSL_MEMCPY(data, sslData, cpylen);
		if (cpylen < len) {
			ssl_oldData = &sslData[cpylen];
			ssl_oldDataLength = len - cpylen;
			ssl_oldDataBase = sslData;
			ssl_oldDataLengthBase = len;
		} else {
			matrixSslProcessedData(ssl, &sslData, (uint32*) &len);
		}
		debug_printf("ssl_read_internal() Bytes rec: %i, copied %i\n", len,
				cpylen);

	} else {
		return SSL_RESULT__WOULD_BLOCK;
	}
	return cpylen;
}

