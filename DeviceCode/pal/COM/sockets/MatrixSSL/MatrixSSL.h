////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _SOCKETS_MATRIXSSL_H_
#define _SOCKETS_MATRIXSSL_H_ 1

//--//

#include "tinyhal.h"
#include "matrixssllib.h"

//--//

//--//
#define SSL_SOCKET_ATTEMPTED_CONNECT -1
#define MATRIXSSL_USE_SESSION_ID 1

#define MATRIXSSL_PRINTF(x ...) TINYCLR_SSL_PRINTF(x)
#define MATRIXSSL_PERROR(x ...) MATRIXSSL_PRINTF(x)
#define MATRIXSSL_PDEBUG(x ...) MATRIXSSL_PRINTF(x)

//--//

typedef unsigned char * X509Certificate;

typedef struct {
	X509Certificate cert;
	int size;
} RTCertificate;



typedef struct SSL_Conext {
		void* SslContext;
		sslSessionId_t* SslSessionId;
		INT32 SocketHandle;
		INT32 CryptokiSession;

		//RTCertificate m_certificates[SSL_Driver.c_MaxCertificatesPerContext];
		INT32 m_certificateCount;

	}SSL_Context_ts;

struct SSL_Driver {
	static const int c_MaxSslContexts = SOCKETS_MAX_COUNT;
	static const int c_MaxCertificatesPerContext = 10;

	//--//



	SSL_Context_ts m_sslContextArray[c_MaxSslContexts];
	INT32 m_sslContextCount;
	SSL_DATE_TIME_FUNC m_pfnGetTimeFuncPtr;

	INT32 SearchFreeSslContextIndex() {
		int freeIndex = -1;

		for (int i = 0; i < ARRAYSIZE(this->m_sslContextArray); i++) {
			if (this->m_sslContextArray[i].SslContext == NULL) {
				freeIndex = i;
				break;
			}
		}
		return freeIndex;
	}

	void AddSslContext(INT32 sslIndex, void* sslContext) {
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SslContext = sslContext;
			this->m_sslContextCount++;
		} else {
			MATRIXSSL_PERROR(
					"AddSslContext(): index out of bounds (sslIndex: %i)", sslIndex);
		}
	}

	void AddSslSessionId(INT32 sslIndex, sslSessionId_t* sslSessionId) {
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SslSessionId = sslSessionId;
		} else {
			MATRIXSSL_PERROR(
					"AddSslSessionId(): index out of bounds (sslIndex: %i)", sslIndex);
		}
	}

	void AddSslSocketHandle(INT32 sslIndex, INT32 socketHandle) {
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SocketHandle = socketHandle;
		} else {
			MATRIXSSL_PERROR(
					"AddSslSocketHandle(): index out of bounds (sslIndex: %i)", sslIndex);
		}
	}

	bool CheckIndex(INT32 sslIndex) {
		if ((sslIndex >= c_MaxSslContexts) || (sslIndex < 0)) {
			return false;
		}
		return true;
	}

	SSL_Context_ts* GetSSLContextBySocketHandle(INT32 socketHandle) {
		INT32 sslcontexIndex = 0;
		for (INT32 i = 0; i < c_MaxSslContexts; i++) {
			if (this->m_sslContextArray[i].SocketHandle == socketHandle) {
				return &(this->m_sslContextArray[i]);
			}
		}
		return NULL;
	}

	SSL_Context_ts* GetSSLContextBySslIndex(INT32 sslIndex) {
		if (CheckIndex(sslIndex) == true) {
			return &(this->m_sslContextArray[sslIndex]);
		} else {
			MATRIXSSL_PERROR(
					"GetSSLContextBySslIndex(): index out of bounds (sslIndex: %i)", sslIndex);
		}
		return NULL;
	}

};

extern SSL_Driver g_SSL_Driver;

#endif //_SOCKETS_OPENSSL_H_
