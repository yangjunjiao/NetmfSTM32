////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _SOCKETS_MATRIXSSL_H_
#define _SOCKETS_MATRIXSSL_H_ 1

//--//

#include "tinyhal.h"
#include "matrixssllib.h"
#include "matrixsslApi.h"

//--//

//--//
#define SSL_SOCKET_ATTEMPTED_CONNECT -1
//#define MATRIXSSL_USE_SESSION_ID 1

#define MATRIXSSL_DEBUG_LEVEL 2
#define MATRIXSSL_PRINTF(x ...) TINYCLR_SSL_PRINTF(x)
#define MATRIXSSL_PERROR(x ...) {MATRIXSSL_PRINTF("ERROR: %s(), line: %i\t", __FUNCTION__, __LINE__); MATRIXSSL_PRINTF(x);}
#define MATRIXSSL_PDEBUG(x ...) {MATRIXSSL_PRINTF("%s(), line:  %i\t", __FUNCTION__, __LINE__); MATRIXSSL_PRINTF(x);}
#if MATRIXSSL_DEBUG_LEVEL > 1
#define MATRIXSSL_PDEBUG_ALL(x ...) {MATRIXSSL_PRINTF("%s(), line:  %i\t", __FUNCTION__, __LINE__); MATRIXSSL_PRINTF(x);}
#else
#define MATRIXSSL_PDEBUG_ALL(x ...)
#endif
//--//

typedef unsigned char * X509Certificate;

typedef struct {
	X509Certificate cert;
	int size;
} RTCertificate;


typedef struct SSL_Buffer{
	unsigned char* BaseBuffer;
	unsigned char* ReadBuffer;
	INT32 Length;
	INT32 Count;

	void Clear()
		{
			BaseBuffer = NULL;
			ReadBuffer = NULL;
			Length = 0;
			Count = 0;
		}
	void Init(unsigned char* base, INT32 length)
	{
		BaseBuffer = base;
		ReadBuffer = base;
		Length = length;
		Count = length;
	}
	INT32 Read(unsigned char* data, INT32 size)
	{
		INT32 cpylen = min(size, Count);
		TINYCLR_SSL_MEMCPY(data, Read(cpylen), cpylen);
		return cpylen;
	}
	unsigned char* Read(INT32 count)
	{
		unsigned char* result = ReadBuffer;
		ReadBuffer = &ReadBuffer[count];
		Count -= count;
		return result;
	}
	bool DataAvailable()
	{
		return Count > 0;
	}
	void Update()
	{
		ReadBuffer = BaseBuffer;
		Count = Length;
	}

} SSL_Buffer_ts;

typedef struct SSL_Conext {
		ssl_t* SslContext;
#ifdef MATRIXSSL_USE_SESSION_ID
		sslSessionId_t* SslSessionId;
#endif
		INT32 SocketHandle;
		INT32 CryptokiSession;
		SSL_Buffer_ts SslBuffer;

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
		if(freeIndex == -1)
		{
			MATRIXSSL_PERROR("No free SSL-Context\n");
		}
		return freeIndex;
	}

	void CreateSslContext(INT32 sslIndex, ssl_t* sslContext) {
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SslContext = sslContext;
#ifdef MATRIXSSL_USE_SESSION_ID
//			this->m_sslContextArray[sslIndex].SslSessionId =  (sslSessionId_t*)TINYCLR_SSL_MALLOC(sizeof(sslSessionId)); -->moved to init
#endif
			this->m_sslContextCount++;
		} else {
			MATRIXSSL_PERROR(
					"index out of bounds (sslIndex: %i)", sslIndex);
		}
	}
	void DeleteSslContext(INT32 sslIndex)
	{
		if (CheckIndex(sslIndex) == true) {
		TINYCLR_SSL_MEMSET((void* )&this->m_sslContextArray[sslIndex], 0, sizeof(this->m_sslContextArray[sslIndex]));
#ifdef MATRIXSSL_USE_SESSION_ID
		if(this->m_sslContextArray[sslIndex].SslSessionId != NULL)
		{
			TINYCLR_SSL_FREE (this->m_sslContextArray[sslIndex].SslSessionId);
		}
#endif
		this->m_sslContextCount --;
		} else {
			MATRIXSSL_PERROR(
					"index out of bounds (sslIndex: %i)", sslIndex);
		}
	}

	void AddSslSessionId(INT32 sslIndex, sslSessionId_t* sslSessionId) {
#ifdef MATRIXSSL_USE_SESSION_ID
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SslSessionId = sslSessionId;
		} else {
			MATRIXSSL_PERROR(
					"index out of bounds (sslIndex: %i)", sslIndex);
		}
#endif
	}

	void AddSslSocketHandle(INT32 sslIndex, INT32 socketHandle) {
		if (CheckIndex(sslIndex) == true) {
			this->m_sslContextArray[sslIndex].SocketHandle = socketHandle;
		} else {
			MATRIXSSL_PERROR(
					"index out of bounds (sslIndex: %i)", sslIndex);
		}
	}

	bool CheckIndex(INT32 sslIndex) {
		if ((sslIndex >= c_MaxSslContexts) || (sslIndex < 0)) {
			return false;
		}
		return true;
	}

	SSL_Context_ts* GetSSLContextBySocketHandle(INT32 socketHandle) {
		for (INT32 i = 0; i < c_MaxSslContexts; i++) {
			if (this->m_sslContextArray[i].SocketHandle == socketHandle) {
				return &(this->m_sslContextArray[i]);
			}
		}
		MATRIXSSL_PERROR("Handle not found (socketHandle: %i)", socketHandle);
		return NULL;
	}

	SSL_Context_ts* GetSSLContextBySslIndex(INT32 sslIndex) {
		if (CheckIndex(sslIndex) == true) {
			return &(this->m_sslContextArray[sslIndex]);
		} else {
			MATRIXSSL_PERROR(
					"index out of bounds (sslIndex: %i)", sslIndex);
		}
		return NULL;
	}

};

extern SSL_Driver g_SSL_Driver;

#endif //_SOCKETS_OPENSSL_H_
