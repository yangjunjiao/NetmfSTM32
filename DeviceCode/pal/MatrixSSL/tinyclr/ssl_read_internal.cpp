#include "ssl_functions.h"
#include "adapt/adapt.h"

#define USE_BUFFER

int handleBufferdData(SSL_Conext* sslContext, char* data, size_t size) {
	if (sslContext->SslBuffer.DataAvailable()) {
		MATRIXSSL_PDEBUG("oldData: %i\n", sslContext->SslBuffer.Count);
		int cpylen = sslContext->SslBuffer.Read((unsigned char*) data, size);
		if (sslContext->SslBuffer.DataAvailable() == false) {
			//everything copied
			int rc = matrixSslProcessedData(sslContext->SslContext,
					&sslContext->SslBuffer.BaseBuffer,
					(uint32*) &sslContext->SslBuffer.Length);
			PRINT_RETURN_VALUE(rc);
			//MATRIXSSL_PDEBUG(" oldData processed: %i\n", cpylen);
			if (rc == MATRIXSSL_APP_DATA) {
				MATRIXSSL_PDEBUG("oldData processed, still data available\n");
				sslContext->SslBuffer.Update();
			}
		}
		//MATRIXSSL_PDEBUG("oldData returned: %i\n", cpylen);
		if (cpylen != 0) {
			//ssl_pending_internal(socket); //just to jeck wether new data is available
			return cpylen;
		}
	}
	return 0;
}

int handleAppData(size_t size, int len, char* data, unsigned char* sslData,
		SSL_Conext* sslContext) {
	int rc;
	int cpylen = min(size, len);
	TINYCLR_SSL_MEMCPY(data, sslData, cpylen);
	if (cpylen < len) {
		sslContext->SslBuffer.ReadBuffer = &sslData[cpylen];
		sslContext->SslBuffer.Count = len - cpylen;
		sslContext->SslBuffer.BaseBuffer = sslData;
		sslContext->SslBuffer.Length = len;
		debug_printf("ssl_read_internal() Bytes rec: %i, returend %i\n", len,
				cpylen);
	} else {
		rc = matrixSslProcessedData(sslContext->SslContext, &sslData,
				(uint32*) &len);
		PRINT_RETURN_VALUE(rc);
		if (rc == MATRIXSSL_APP_DATA) {
			sslContext->SslBuffer.Init(sslData, len);
			debug_printf("ssl_read_internal() oldData count: %i,\n", len);
		}
	}
	return cpylen;
}

int handleRequestSend(SSL* ssl, unsigned char*& sslData,
		SSL_Conext*& sslContext, int rc) {
	int len = matrixSslGetOutdata(ssl, &sslData);
	int sent = SOCK_send(sslContext->SocketHandle, (const char*) (sslData), len,
			0);
	rc = matrixSslSentData(sslContext->SslContext, sent);
	PRINT_RETURN_VALUE(rc);
	return rc;
}

int ssl_read_internal(int socket, char* data, size_t size) {

	int rc = 0;
	debug_printf("\nssl_read_internal() size: %i\n", size);
	unsigned char *sslData;
	SSL* ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		return SOCK_SOCKET_ERROR;
	}
	rc = handleBufferdData(sslContext, data, size);
	if (rc > 0) {
		//if data was buffered, return it and wait for next call to get new data from socket
		return rc;
	}

	int len = 0;

	len = matrixSslGetReadbuf(sslContext->SslContext, &sslData);

	int rec = SOCK_recv(sslContext->SocketHandle, (char*) sslData, len, 0);
	debug_printf("ssl_read_internal() SOCK_recv: %i\n", rec);
	if (rec == 0) {
		return rec;
	}
	rc = matrixSslReceivedData(sslContext->SslContext, (int32) rec, &sslData, (uint32*) &len);
	PRINT_RETURN_VALUE(rc);
	if (rc < 0) {
		return SOCK_SOCKET_ERROR;
	}
	debug_printf("ssl_read_internal() matrixSslReceivedData: %i\n", len);
	if (!(rc == MATRIXSSL_APP_DATA || rc == MATRIXSSL_REQUEST_RECV
			|| rc == MATRIXSSL_REQUEST_SEND || rc == MATRIXSSL_RECEIVED_ALERT)) {
		return SOCK_SOCKET_ERROR;
	}

	if (rc == MATRIXSSL_RECEIVED_ALERT) {
		if (len == 2) {
			unsigned char alertLevel = sslData[0];
			unsigned char alertDescription = sslData[1];
			debug_printf(
					"ssl_read_internal() Alert: Level %i, Description: %i\n",
					alertLevel, alertDescription);
			if (alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
				return 0;
			}
		}
		return SOCK_SOCKET_ERROR;
	} else if (rc == MATRIXSSL_REQUEST_SEND) {
		int rc = handleRequestSend(ssl, sslData, sslContext, rc);
		if (rc == MATRIXSSL_SUCCESS) {
			return 0;
		}
		return SOCK_SOCKET_ERROR;
	} else if (rc == MATRIXSSL_APP_DATA) {
		return handleAppData(size, len, data, sslData, sslContext);

	} else {
		return SSL_RESULT__WOULD_BLOCK;
	}
	return 0;
}

