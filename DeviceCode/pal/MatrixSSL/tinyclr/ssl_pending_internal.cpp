#include "ssl_functions.h"
#include "adapt/adapt.h"


int ssl_pending_internal(int socket) {
	SSL* ssl = NULL;
		SSL_Conext* sslContext = g_SSL_Driver.GetSSLContextBySocketHandle(socket);
		if (sslContext != NULL && sslContext->SslContext != NULL) {
			ssl = (SSL*) sslContext->SslContext;
		} else {
			return SOCK_SOCKET_ERROR;
		}

	int len = 0;
	int sslBufferCount = sslContext->SslBuffer.Count;
	//TODO make sure, not using rtip include
	const long fionread = (0x40000000UL|(((long)sizeof(unsigned long)&0x7fU)<<16)|(('f')<<8)|(127));
	//TODO Check for block see SSL.cpp
	int ret = SOCK_ioctl(socket, fionread, &len);
	if (ret == 0)
	{
		MATRIXSSL_PDEBUG_ALL("Bytes in ETH Buffer: %i, in SSL-Buffer: %i\n", len, sslBufferCount);
		//return (len > 0);
		return (len + sslBufferCount);
	}
	return 0;
}

