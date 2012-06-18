#include "stdafx.h"
#include "parseCerts.h"
#include <string.h>



static char* delimiterCertificate = "CERTIFICATE-----";
static char* delimiterKey = "KEY-----";

CertStorage certStorage;


/*
char* eliminateSpecialCharacters(char* in)
{
while (*in == '\x0d' || *in == '\x0a' || *in == '\x09'
|| *in == ' ') {
in++;
}
return in;
}

char* eliminateSpecialCharactersEnd(char* in)
{

do
{
in--;
}
while(*in == '\x0d' || *in == '\x0a' || *in == '\x09'
|| *in == ' ');
return ++in;
}

*/

/**
@ remarks:  out has to be freed
**/
int32 parseBase64Encoded(const unsigned char *in, int32 len, 
    unsigned char **out, int32 *outlen)
{
    int32 result = PS_FAILURE;
    *out = (unsigned char *) psMalloc(NULL, len);
    *outlen = len;
    result = psBase64decode(in, len, *out, (uint32*)outlen);
    return result;
}



int32 getData(const unsigned char *in, uint32 len, unsigned char **out, int32 *outlen, const char* delimiter)
{
    char* start;
    char* end;
    char* endNext;
    int32 currentLength;
    int i = 0;
    while (i++ < len) {
        if (((start = strstr((char*)in, "-----BEGIN")) != NULL) &&
            ((start = strstr((char*)in, delimiter)) != NULL) &&
            ((end = strstr(start, "-----END")) != NULL) &&
            ((endNext = strstr(end,delimiter)) != NULL)) {
                start += strlen(delimiter);
                //start = eliminateSpecialCharacters(start);
                //end = eliminateSpecialCharactersEnd(end);

                currentLength = (int32)(end - start);

                return parseBase64Encoded((unsigned char*) start, currentLength, out, outlen);
                /*
                while (*end == '\x0d' || *end == '\x0a' || *end == '\x09'
                || *end == ' ') {
                end++;
                }
                len -= (int32)((unsigned char*)end - in);*/
        }
    }
    return PS_FAILURE;
}

int32 parseServerCert(const unsigned char *in, uint32 len)
{
    int32 result = PS_SUCCESS;
    result = getData(in, len, &certStorage.certBuf, &certStorage.certLen, delimiterCertificate);
    if (result == PS_FAILURE)
    {
        printf("Parse cert error!\n");
    }
    result = getData(in, len, &certStorage.privBuf, &certStorage.privLen, delimiterKey);
    if (result == PS_FAILURE)
    {
        printf("Parse key error!\n");
    }

    return result;
}

int32 parseClientCA(const unsigned char *in, uint32 len)
{
    int32 result = PS_SUCCESS;
    result = getData(in, len, &certStorage.trustedCABuf, &certStorage.trustedCALen, delimiterCertificate);
    if (result == PS_FAILURE)
    {
        printf("Parse cert error!\n");
    }
    return result;
}

int32 LoadRsaKeys(sslKeys_t * keys)
{
    if (matrixSslLoadRsaKeysMem(keys, certStorage.certBuf, certStorage.certLen,
        certStorage.privBuf, certStorage.privLen, certStorage.trustedCABuf, certStorage.trustedCALen) < 0) {
            return PS_FAILURE;
    }
    return PS_SUCCESS;
}