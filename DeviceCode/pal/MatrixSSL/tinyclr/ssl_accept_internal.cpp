#include "ssl_functions.h"
#include "adapt/adapt.h"
extern int handleAppData(size_t size, int len, char* data, unsigned char* sslData,
		SSL_Conext* sslContext);

// @retrun SOCK_SOCKET_ERROR, SOCK_TRY_AGAIN and 0 if successful
int ssl_accept_internal(int socket, int sslContextHandle) {


	int ret = SOCK_SOCKET_ERROR;
	//int done = 0;
	SSL *ssl = NULL;

	MATRIXSSL_PDEBUG_ALL("------------- called ----------------: Socket: %i, SSL: %i\n", socket, sslContextHandle);
	SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySslIndex(
			sslContextHandle);
	if (sslContext != NULL && sslContext->SslContext != NULL) {
		ssl = (SSL*) sslContext->SslContext;
	} else {
		MATRIXSSL_PERROR("Context not valid\n");
		return SOCK_SOCKET_ERROR;
	}
	g_SSL_Driver.AddSslSocketHandle(sslContextHandle, socket);

	unsigned char *sslData;

	int rc = 0;
	int sent = 0;
	int len = matrixSslGetOutdata(ssl, &sslData);

	//do {
		if (len > 0) {

			//if (len > 0) {
				ret = SOCK_TRY_AGAIN;
				sent = SOCK_send(socket, (const char *) sslData, len, 0);
				MATRIXSSL_PDEBUG_ALL("SOCK_send: %i\n", sent);
				rc = matrixSslSentData(ssl, sent);
				PRINT_RETURN_VALUE(rc);
				//done = 1;
				switch (rc) {
				case MATRIXSSL_SUCCESS:
					//ok, continue
					ret = SOCK_TRY_AGAIN;

					break;
				case MATRIXSSL_HANDSHAKE_COMPLETE:
					ret = 0;
					break;
				case MATRIXSSL_REQUEST_SEND:
									ret = SOCK_TRY_AGAIN;
									break;
				default:
					PRINT_UNEXPECTED_RETURN_VALUE(rc);
					ret = SOCK_SOCKET_ERROR;
					break;
				}

			//}
		} else
		{
			if(ssl_pending_internal(socket) > 0)
			{
			int len = 1000;
			len = matrixSslGetReadbuf(ssl, &sslData);
			if(len == 0)
			{
				MATRIXSSL_PERROR("matrixSslGetReadbuf returned 0\n");
				ret = SOCK_SOCKET_ERROR;
			}
			len = SOCK_recv(socket, (char*) sslData, len, 0);
			MATRIXSSL_PDEBUG_ALL("SOCK_recv: %i\n", len);
			ret = SOCK_TRY_AGAIN;
			if (len > 0) {
				rc = matrixSslReceivedData(ssl, (int32) len, &sslData,
						(uint32*) &len);
				PRINT_RETURN_VALUE(rc);

				switch (rc) {
				case MATRIXSSL_SUCCESS:
					//ok, continue
					//ret = SOCK_TRY_AGAIN;
					break;
				case MATRIXSSL_REQUEST_SEND:
					ret = SOCK_TRY_AGAIN;
					break;
				case MATRIXSSL_HANDSHAKE_COMPLETE:
					ret = 0;
					break;
				case MATRIXSSL_APP_DATA: //partial record remaining. If there were a full SSL record available, for example an application data record, it would be parsed and MATRIXSSL_APP_DATA would be returned instead. see API-Doc page 23
					handleAppData(0, len, NULL, sslData, sslContext);
					ret = 0;
					break;
				default:
					PRINT_UNEXPECTED_RETURN_VALUE(rc);
					ret = SOCK_SOCKET_ERROR;
					break;
				}
			}
			}
			else
			{
				ret = SOCK_TRY_AGAIN;
			}

		}
		/*if (done == 0) {
			len = matrixSslGetOutdata(ssl, &sslData);
			PRINT_RETURN_VALUE(rc);
		} else {
			len = 0;
		}*/
	//} while (len);

	if (ret == 0) //if handshake is successfully completed
			{
		//g_SSL_Driver.AddSslSocketHandle(sslContextHandle, socket);
	}

	return ret;

	/*int len = 0;

	 len = matrixSslGetReadbuf(sslContext->SslContext, &sslData);

	 int rec = SOCK_recv(sslContext->SocketHandle, (char*) sslData, len, 0);
	 MATRIXSSL_PDEBUG_ALL("SOCK_recv: %i\n", rec);
	 if (rec == 0) {
	 return SOCK_SOCKET_ERROR;
	 }
	 rc = matrixSslReceivedData(sslContext->SslContext, (int32) rec, &sslData, (uint32*) &len);
	 PRINT_RETURN_VALUE(rc);
	 if (rc < 0) {
	 PRINT_UNEXPECTED_RETURN_VALUE(rc);
	 return SOCK_SOCKET_ERROR;
	 }
	 else
	 {
	 if (rc == MATRIXSSL_REQUEST_RECV) {
	 status = MATRIXSSL_REQUEST_RECV;
	 }
	 if (rc == MATRIXSSL_REQUEST_SEND) {
	 status = MATRIXSSL_REQUEST_SEND;
	 return SOCK_TRY_AGAIN;
	 }
	 if (rc == MATRIXSSL_HANDSHAKE_COMPLETE) {
	 return 0;
	 }
	 }

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
	 }*/

	/*
	 ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;
	 if (ssl == NULL)
	 {
	 goto error;
	 }

	 if (!SSL_set_fd(ssl, sd))
	 {
	 goto error;
	 }

	 SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

	 err = SSL_accept (ssl);

	 nonblock = 1;
	 SOCK_ioctl(sd, SOCK_FIONBIO, &nonblock);

	 err = SSL_get_error(ssl,err);

	 switch(err)
	 {
	 case SSL_ERROR_WANT_READ:
	 err = SOCK_EWOULDBLOCK;
	 #ifndef TCPIP_LWIP
	 SOCKET_DRIVER.ClearStatusBitsForSocket( sd, FALSE );
	 #endif
	 break;

	 case SSL_ERROR_WANT_WRITE:
	 err = SOCK_TRY_AGAIN;
	 #ifndef TCPIP_LWIP
	 SOCKET_DRIVER.ClearStatusBitsForSocket( sd, TRUE );
	 #endif
	 break;

	 case SSL_ERROR_NONE:
	 #ifndef TCPIP_LWIP
	 SOCKET_DRIVER.ClearStatusBitsForSocket( sd, FALSE );
	 #endif
	 break;
	 }

	 SOCKET_DRIVER.SetSocketSslData(sd, (void*)ssl);
	 */

	return SOCK_SOCKET_ERROR;
}

