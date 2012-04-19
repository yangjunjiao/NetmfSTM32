#include "ssl_functions.h"
#include "adapt/adapt.h"


#define USE_BUFFER



#ifdef USE_BUFFER
unsigned char* ssl_oldData = NULL;
int ssl_oldDataLength = 0;
unsigned char* ssl_oldDataBase = NULL;
int ssl_oldDataLengthBase = 0;
#endif

int ssl_read_internal(int socket, char* data, size_t size) {

	int rc = 0;
	debug_printf("\nssl_read_internal() size: %i\n", size);
	unsigned char *sslData;
	//SSL* ssl = (SSL*) g_SSL_Driver.m_sslContextArray[0].SslContext;
	SSL* ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		return SOCK_SOCKET_ERROR;
	}
#ifdef USE_BUFFER
	if (ssl_oldDataLength != 0) {
		debug_printf("ssl_read_internal() oldData: %i\n", ssl_oldDataLength);
		int cpylen = min(size, ssl_oldDataLength);
		TINYCLR_SSL_MEMCPY(data, ssl_oldData, cpylen);
		if (cpylen == ssl_oldDataLength) {
			//everything copied
			ssl_oldDataLength = 0;
			ssl_oldData = NULL;
			rc = matrixSslProcessedData(ssl, &ssl_oldDataBase,
					(uint32*) &ssl_oldDataLengthBase);
			PRINT_RETURN_VALUE(rc);
			debug_printf("ssl_read_internal() oldData processed: %i\n", cpylen);
			if(rc == MATRIXSSL_APP_DATA)
			{
				debug_printf("ssl_read_internal() oldData processed: still data available\n");
				ssl_oldData = ssl_oldDataBase;
				ssl_oldDataLength = ssl_oldDataLengthBase;
			}
		} else {
			ssl_oldData = &ssl_oldData[cpylen];
			ssl_oldDataLength = ssl_oldDataLength - cpylen;
		}
		debug_printf("ssl_read_internal() oldData returned: %i\n", cpylen);
		if (cpylen != 0)
			return cpylen;
	}
#endif
	if (ssl == NULL) {
		return SOCK_SOCKET_ERROR;
	}

	int len = 0;

	len = matrixSslGetReadbuf(ssl, &sslData);
#ifdef 0
	len = min (len, size);

	if (len < 10)
		len = 100;
#endif
	int rec = SOCK_recv(socket, (char*) sslData, len, 0);
	debug_printf("ssl_read_internal() SOCK_recv: %i\n", rec);
	if (rec == 0) {
		return rec;
	}
	rc = matrixSslReceivedData(ssl, (int32) rec, &sslData, (uint32*) &len);
	PRINT_RETURN_VALUE(rc);
	if (rc < 0) {
		return SOCK_SOCKET_ERROR;
	}
	debug_printf("ssl_read_internal() matrixSslReceivedData: %i\n", len);
	if (!(rc == MATRIXSSL_APP_DATA || rc == MATRIXSSL_REQUEST_RECV
			|| rc == MATRIXSSL_REQUEST_SEND || rc == MATRIXSSL_RECEIVED_ALERT)) {
		return SOCK_SOCKET_ERROR;
	}


	int cpylen = min (size, len);
	if(rc == MATRIXSSL_RECEIVED_ALERT)
	{
		if(len == 2)
		{
			unsigned char alertLevel = sslData[0];
			unsigned char alertDescription = sslData[1];
			debug_printf("ssl_read_internal() Alert: Level %i, Description: %i\n", alertLevel, alertDescription);
			if(alertDescription == SSL_ALERT_CLOSE_NOTIFY)
			{
				return 0;
			}
		}
		return SOCK_SOCKET_ERROR;
	}
	if (rc == MATRIXSSL_REQUEST_SEND) {
		int len = matrixSslGetOutdata(ssl, &sslData);

		int sent = SOCK_send(socket, (const char *) sslData, len, 0);
		int rc = matrixSslSentData(ssl, sent);
		PRINT_RETURN_VALUE(rc);
		if (rc == MATRIXSSL_SUCCESS) {
			return 0;
		}
		return 0;
	}
	if (rc == MATRIXSSL_APP_DATA) {
		TINYCLR_SSL_MEMCPY(data, sslData, cpylen);
#ifdef USE_BUFFER
		if (cpylen < len) {
			ssl_oldData = &sslData[cpylen];
			ssl_oldDataLength = len - cpylen;
			ssl_oldDataBase = sslData;
			ssl_oldDataLengthBase = len;
		} else {
			rc = matrixSslProcessedData(ssl, &sslData, (uint32*) &len);
			PRINT_RETURN_VALUE(rc);
		}
#else
		rc = matrixSslProcessedData(ssl, &sslData, (uint32*) &cpylen);
		PRINT_RETURN_VALUE(rc);
#endif

		debug_printf("ssl_read_internal() Bytes rec: %i, returend %i\n", len,
				cpylen);

	} else {
		return SSL_RESULT__WOULD_BLOCK;
	}
	return cpylen;
}

