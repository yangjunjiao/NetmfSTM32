#include "ssl_functions.h"
#include "adapt/adapt.h"

//extern CK_RV Cryptoki_GetSlotIDFromSession(CK_SESSION_HANDLE session, CK_SLOT_ID_PTR pSlotID, CryptokiSession** ppSession);

#define SSL_SOCKET_ATTEMPTED_CONNECT -1

#define RTSSL_WANT_WRITE 33
#define RTSSL_WANT_READ 44
int ssl_connect_internal(int socket, const char* szTargetHost, int sslContextHandle)
{
	static int status = MATRIXSSL_REQUEST_SEND ;
    int ret = SOCK_SOCKET_ERROR;
    SSL *ssl = NULL;
    int nonblock = 0;
    int done = 0;

    // Retrieve SSL struct from g_SSL_Driver    
    /*if((sslContextHandle >= ARRAYSIZE(g_SSL_Driver.m_sslContextArray)) || (sslContextHandle < 0))
    {
        goto error;
    }*/
    

    // sd should already have been created
    // Now do the SSL negotiation   
    SSL_Conext* sslContext =g_SSL_Driver.GetSSLContextBySslIndex(sslContextHandle);
    if(sslContext != NULL && sslContext->SslContext != NULL)
    {
    	ssl = (SSL*) sslContext->SslContext;
    }
    //ssl = (SSL*)g_SSL_Driver.m_sslContextArray[sslContextHandle].SslContext;

    if (ssl == NULL) goto error;

    unsigned  char  *sslData;





    // WARNING - SSL_Connect is asynchronous and will be called multiple times for 1 connection, therefore
        // we only want to set the CA store on the first call (when sslData == NULL)
        //
        // The first certificate is the device's outbound certificate
        if(sslData == NULL )
        {
            	//First time called
               // return SOCK_SOCKET_ERROR;


        }

        if(sslData == (void*)SSL_SOCKET_ATTEMPTED_CONNECT)
        {
            sslData = NULL;
        }
        int rc = 0;
        int sent = 0;
        int len = matrixSslGetOutdata(ssl, &sslData);
        do
        {
        if (len > 0) //(status == MATRIXSSL_REQUEST_SEND )
        {
			//int len = matrixSslGetOutdata(ssl, &sslData);
			if (len > 0)
			{
				ret = RTSSL_WANT_WRITE;
				sent = SOCK_send(socket, (const char * )sslData, len, 0);
			    rc = matrixSslSentData(ssl, sent);
			    done = 1;
                if (rc == MATRIXSSL_SUCCESS)
                {
                    status = MATRIXSSL_REQUEST_RECV;
                }
			}
        }
        else //if(status == MATRIXSSL_REQUEST_RECV && done == 0)
        {
        	int len = 1000;
        	len = matrixSslGetReadbuf(ssl, &sslData);
        	len = SOCK_recv(socket, (char*) sslData, len, 0);
            ret = RTSSL_WANT_READ;
        	if(len > 0)
        	{
        		 rc = matrixSslReceivedData(ssl, (int32)len, &sslData,
        											(uint32*)&len);
                 if (rc == MATRIXSSL_REQUEST_RECV)
                {
                    status = MATRIXSSL_REQUEST_RECV;
                }
                if (rc == MATRIXSSL_REQUEST_SEND )
                {
                    status = MATRIXSSL_REQUEST_SEND ;
                    ret = RTSSL_WANT_WRITE;
                }
                if( rc == MATRIXSSL_HANDSHAKE_COMPLETE)
                {
                	ret = 0;
                }
        	}
        }
        if(done == 0)
        {
        	len = matrixSslGetOutdata(ssl, &sslData);
        }
        else
        {
        	len = 0;
        }
        } while (len);


       // ret = secure_connect( socket, szTargetHost, ctx->SslContext, &sslData );

        if(ret == RTSSL_WANT_READ)
        {
            ret = SOCK_EWOULDBLOCK;

            //SOCKET_DRIVER.ClearStatusBitsForSocket( socket, FALSE );
        }

        if(ret == RTSSL_WANT_WRITE)
        {
            ret = SOCK_TRY_AGAIN;

            //SOCKET_DRIVER.ClearStatusBitsForSocket( socket, TRUE );
        }

        if(sslData != NULL)
        {
        	//SOCKET_DRIVER.SetSocketSslData(socket, sslData);
        }
        // make sure we call secure_close_socket if we attempted a connect
        else
        {
          //  g_Sockets_Driver.SetSocketSslData(socket, (void*)SSL_SOCKET_ATTEMPTED_CONNECT);
        }

        //SOCKET_DRIVER.ClearStatusBitsForSocket( socket, FALSE );
        //SOCKET_DRIVER.ClearStatusBitsForSocket( socket, TRUE );


       g_SSL_Driver.AddSslSocketHandle(sslContextHandle, socket);

error:
    return ret;
}

