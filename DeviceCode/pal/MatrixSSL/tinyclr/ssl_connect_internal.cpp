#include "ssl_functions.h"
#include "adapt/adapt.h"

//extern CK_RV Cryptoki_GetSlotIDFromSession(CK_SESSION_HANDLE session, CK_SLOT_ID_PTR pSlotID, CryptokiSession** ppSession);

#define SSL_SOCKET_ATTEMPTED_CONNECT -1


int ssl_connect_internal(int socket, const char* szTargetHost,
		int sslContextHandle) {
	static int status = MATRIXSSL_REQUEST_SEND;
	int ret = SOCK_SOCKET_ERROR;

	int nonblock = 0;
	int done = 0;

	SSL *ssl = NULL;
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySslIndex(
			sslContextHandle);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		MATRIXSSL_PERROR("Context not valid\n");
		return SOCK_SOCKET_ERROR;
	}

	unsigned char *sslData;

	// WARNING - SSL_Connect is asynchronous and will be called multiple times for 1 connection, therefore
	// we only want to set the CA store on the first call (when sslData == NULL)
	//
	// The first certificate is the device's outbound certificate

	int rc = 0;
	int sent = 0;
	int len = matrixSslGetOutdata(ssl, &sslData);
	do {
		if (len > 0) //(status == MATRIXSSL_REQUEST_SEND )
				{

				ret = SOCK_TRY_AGAIN;
				sent = SOCK_send(socket, (const char *) sslData, len, 0);
				//TODO check if sent
				rc = matrixSslSentData(ssl, sent);
				done = 1;
				if (rc == MATRIXSSL_SUCCESS) {
					status = MATRIXSSL_REQUEST_RECV;

			}
		} else //if(status == MATRIXSSL_REQUEST_RECV && done == 0)
		{
			int len = 1000;
			len = matrixSslGetReadbuf(ssl, &sslData);
			len = SOCK_recv(socket, (char*) sslData, len, 0);
			ret = SOCK_TRY_AGAIN;
			if (len > 0) {
				rc = matrixSslReceivedData(ssl, (int32) len, &sslData,
						(uint32*) &len);
				if (rc == MATRIXSSL_REQUEST_RECV) {
					status = MATRIXSSL_REQUEST_RECV;
				}
				if (rc == MATRIXSSL_REQUEST_SEND) {
					status = MATRIXSSL_REQUEST_SEND;
					ret = SOCK_TRY_AGAIN;
				}
				if (rc == MATRIXSSL_HANDSHAKE_COMPLETE) {
					ret = 0;
				}
			}
		}
		if (done == 0) {
			len = matrixSslGetOutdata(ssl, &sslData);
		} else {
			len = 0;
		}
	} while (len);


	g_SSL_Driver.AddSslSocketHandle(sslContextHandle, socket);


	return ret;
}

