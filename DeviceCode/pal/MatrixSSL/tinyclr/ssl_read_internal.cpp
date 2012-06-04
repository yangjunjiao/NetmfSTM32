#include "ssl_functions.h"
#include "adapt/adapt.h"

#define USE_BUFFER

int handleRequestSend(SSL_Conext*& sslContext);
int handleBufferdData(SSL_Conext* sslContext, char* data, size_t size);
int handleAppData(size_t size, int len, char* data, unsigned char* sslData,
		SSL_Conext* sslContext);

int handleBufferdData(SSL_Conext* sslContext, char* data, size_t size) {
	if (sslContext->SslBuffer.DataAvailable()) {
		MATRIXSSL_PDEBUG_ALL("oldData: %i\n", sslContext->SslBuffer.Count);
		int cpylen = sslContext->SslBuffer.Read((unsigned char*) data, size);
		if (sslContext->SslBuffer.DataAvailable() == false) {
			//everything copied
			int rc = matrixSslProcessedData(sslContext->SslContext,
					&sslContext->SslBuffer.BaseBuffer,
					(uint32*) &sslContext->SslBuffer.Length);
			PRINT_RETURN_VALUE(rc);
			//MATRIXSSL_PDEBUG(" oldData processed: %i\n", cpylen);
			if (rc == MATRIXSSL_APP_DATA) {
				MATRIXSSL_PDEBUG_ALL(
						"oldData processed, still data available\n");
				sslContext->SslBuffer.Update();
			} else if (rc == MATRIXSSL_REQUEST_SEND) {
				int rc = handleRequestSend(sslContext);
				if (rc != MATRIXSSL_SUCCESS) {
					PRINT_UNEXPECTED_RETURN_VALUE(rc);
					return 0;
				}
			} else if (rc != MATRIXSSL_REQUEST_RECV && rc != MATRIXSSL_SUCCESS) {
				PRINT_UNEXPECTED_RETURN_VALUE(rc);
			}
		}
		//MATRIXSSL_PDEBUG("oldData returned: %i\n", cpylen);
		if (cpylen != 0) {
			//ssl_pending_internal(socket); //just to check whether new data is available
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
		MATRIXSSL_PDEBUG_ALL("Bytes rec: %i, returned %i\n", len, cpylen);
	} else {
		rc = matrixSslProcessedData(sslContext->SslContext, &sslData,
				(uint32*) &len);
		PRINT_RETURN_VALUE(rc);
		if (rc == MATRIXSSL_APP_DATA) {
			sslContext->SslBuffer.Init(sslData, len);
			MATRIXSSL_PDEBUG_ALL("oldData count: %i,\n", len);
		} else if (rc != MATRIXSSL_REQUEST_RECV && rc != MATRIXSSL_SUCCESS) {
			PRINT_UNEXPECTED_RETURN_VALUE(rc);
			return SOCK_SOCKET_ERROR;
		}
	}
	return cpylen;
}

int handleRequestSend(SSL_Conext*& sslContext) {
	unsigned char *sslData;
	int len = matrixSslGetOutdata(sslContext->SslContext, &sslData);
	int sent = SOCK_send(sslContext->SocketHandle, (const char*) (sslData), len,
			0);
	int rc = matrixSslSentData(sslContext->SslContext, sent);
	if (rc == MATRIXSSL_REQUEST_CLOSE || rc == MATRIXSSL_SUCCESS) {
		return MATRIXSSL_SUCCESS;
	}
	if (rc == MATRIXSSL_REQUEST_SEND) {
		MATRIXSSL_PDEBUG("WARNING: Untested function call\n");
		//TODO
		return handleRequestSend(sslContext);
	}
	PRINT_UNEXPECTED_RETURN_VALUE(rc);
	return rc;
}

int ssl_read_internal(int socket, char* data, size_t size) {

	int rc = -1;
	MATRIXSSL_PDEBUG_ALL("size: %i\n", size);
	unsigned char *sslData;
	SSL* ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		return SOCK_SOCKET_ERROR;
	}
	rc = handleBufferdData(sslContext, data, size);
	if (rc > 0 || rc == SOCK_SOCKET_ERROR) {
		//if data was buffered, return it and wait for next call to get new data from socket
		return rc;
	}

	int len = 0;

	len = matrixSslGetReadbuf(sslContext->SslContext, &sslData);

	int rec = SOCK_recv(sslContext->SocketHandle, (char*) sslData, len, 0);
	MATRIXSSL_PDEBUG_ALL("SOCK_recv: %i\n", rec);
	if (rec == 0) {
		return rec;
	}
	rc = matrixSslReceivedData(sslContext->SslContext, (int32) rec, &sslData,
			(uint32*) &len);
	PRINT_RETURN_VALUE(rc);
	if (rc < 0) {
		PRINT_UNEXPECTED_RETURN_VALUE(rc);
		return SOCK_SOCKET_ERROR;
	}
	MATRIXSSL_PDEBUG_ALL("matrixSslReceivedData: %i\n", len);
	if (!(rc == MATRIXSSL_APP_DATA || rc == MATRIXSSL_REQUEST_RECV
			|| rc == MATRIXSSL_REQUEST_SEND || rc == MATRIXSSL_RECEIVED_ALERT)) {
		PRINT_UNEXPECTED_RETURN_VALUE(rc);
		return SOCK_SOCKET_ERROR;
	}

	if (rc == MATRIXSSL_RECEIVED_ALERT) {
		if (len == 2) {
			unsigned char alertLevel = sslData[0];
			unsigned char alertDescription = sslData[1];
			MATRIXSSL_PDEBUG(
					"Alert: Level %i, Description: %i\n", alertLevel, alertDescription);
			if (alertDescription == SSL_ALERT_CLOSE_NOTIFY) {
				return 0;
			}
		}
		return SOCK_SOCKET_ERROR;
	} else if (rc == MATRIXSSL_REQUEST_SEND) {
		int rc = handleRequestSend(sslContext);
		if (rc == MATRIXSSL_SUCCESS) {
			return 0;
		}
		return SOCK_SOCKET_ERROR;
	} else if (rc == MATRIXSSL_APP_DATA) {
		return handleAppData(size, len, data, sslData, sslContext);

	} else {
		return SSL_RESULT__WOULD_BLOCK;
	}
	PRINT_UNEXPECTED_RETURN_VALUE(rc);
	return 0;
}

