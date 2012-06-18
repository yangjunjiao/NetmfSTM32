#ifndef _PARS_CERTS_H_
#define _PARS_CERTS_H_

#include "stdafx.h"
#include <matrixssl/matrixsslApi.h>
extern int32 parseServerCert(const unsigned char *in, uint32 len);
extern int32 parseClientCA(const unsigned char *in, uint32 len);
extern int32 LoadRsaKeys(sslKeys_t * keys);

typedef struct CertStorage
{
    unsigned char *certBuf; 
    int32 certLen;
    unsigned char *privBuf; 
    int32 privLen;
    unsigned char *trustedCABuf; 
    int32 trustedCALen;
    void Init()
    {
        psMemset(this, 0, sizeof(CertStorage));
    }
    void Delete() 
    {
        psFree(certBuf);
        psFree(privBuf);
        psFree(trustedCABuf);
    }
} CertStorage_ts;



#endif  //_PARS_CERTS_H_