#include "ssl_functions.h"
#include "adapt/adapt.h"

//extern CK_RV Cryptoki_GetSlotIDFromSession(CK_SESSION_HANDLE session, CK_SLOT_ID_PTR pSlotID, CryptokiSession** ppSession);

sslSessionId_t sid; //TODO per session

extern sslKeys_t *g_pSslKeys; //TODO

#define ALLOW_ANON_CONNECTIONS	1 //TODO
/*
 //SK see MatrixSSL Client.c
 Example callback to do additional certificate validation.
 If this callback is not registered in matrixSslNewService,
 the connection will be accepted or closed based on the status flag.
 */
static int32 certCb(ssl_t *ssl, psX509Cert_t *cert, int32 alert) {
#ifdef POSIX
	struct tm t;
	time_t rawtime;
	char *c;
	int y, m, d;
#endif

	/* Example to allow anonymous connections based on a define */
	if (alert > 0) {
		if (ALLOW_ANON_CONNECTIONS) {
			TINYCLR_SSL_PRINTF("Allowing anonymous connection for: %s.\n",
					cert->subject.commonName);
			return SSL_ALLOW_ANON_CONNECTION;
		}
		TINYCLR_SSL_PRINTF("Certificate callback returning fatal alert\n");
		return alert;
	}

#ifdef POSIX
	/* Validate the dates in the cert */
	time(&rawtime);
	localtime_r(&rawtime, &t);
	/* Localtime does months from 0-11 and (year-1900)! Normalize it. */
	t.tm_mon++;
	t.tm_year += 1900;

	/* Validate the 'not before' date */
	if ((c = cert->notBefore) != NULL) {
		if (strlen(c) < 8) {
			return PS_FAILURE;
		}
		/* UTCTIME, defined in 1982, has just a 2 digit year */
		if (cert->timeType == ASN_UTCTIME) {
			y = 2000 + 10 * (c[0] - '0') + (c[1] - '0'); c += 2;
		} else {
			y = 1000 * (c[0] - '0') + 100 * (c[1] - '0') +
			10 * (c[2] - '0') + (c[3] - '0'); c += 4;
		}
		m = 10 * (c[0] - '0') + (c[1] - '0'); c += 2;
		d = 10 * (c[0] - '0') + (c[1] - '0');
		if (t.tm_year < y) return PS_FAILURE;
		if (t.tm_year == y) {
			if (t.tm_mon < m) return PS_FAILURE;
			if (t.tm_mon == m && t.tm_mday < d) return PS_FAILURE;
		}
		/*		_psTraceStr("Validated notBefore: %s\n", cert->notBefore); */
	}

	/* Validate the 'not after' date */
	if ((c = cert->notAfter) != NULL) {
		if (strlen(c) < 8) {
			return PS_FAILURE;
		}
		/* UTCTIME, defined in 1982 has just a 2 digit year */
		if (cert->timeType == ASN_UTCTIME) {
			y = 2000 + 10 * (c[0] - '0') + (c[1] - '0'); c += 2;
		} else {
			y = 1000 * (c[0] - '0') + 100 * (c[1] - '0') +
			10 * (c[2] - '0') + (c[3] - '0'); c += 4;
		}
		m = 10 * (c[0] - '0') + (c[1] - '0'); c += 2;
		d = 10 * (c[0] - '0') + (c[1] - '0');
		if (t.tm_year > y) return PS_FAILURE;
		if (t.tm_year == y) {
			if (t.tm_mon > m) return PS_FAILURE;
			if (t.tm_mon == m && t.tm_mday > d) return PS_FAILURE;
		}
		/*		_psTraceStr("Validated notAfter: %s\n", cert->notAfter); */
	}
#endif /* POSIX */
	TINYCLR_SSL_PRINTF("Validated cert for: %s.\n", cert->subject.commonName);

	return PS_SUCCESS;
}

BOOL ssl_generic_init_internal(int sslMode, int sslVerify,
		const char* certificate, int cert_len, const char* szCertPwd,
		int& sslContextHandle, BOOL isServer) {
	//TODO  sslMode, sslVerify, certificate, cert_len, pwd, sslContextHandle
	SSL* ssl = NULL;
	int32 rc;

	int sslCtxIndex = -1;

	for (int i = 0; i < ARRAYSIZE(g_SSL_Driver.m_sslContextArray); i++) {
		if (g_SSL_Driver.m_sslContextArray[i].SslContext == NULL) {
			sslCtxIndex = i;
			break;
		}
	}

	if (sslCtxIndex == -1)
		return FALSE;

	if (isServer == FALSE) {
		matrixSslInitSessionId(sid);
		rc = matrixSslNewClientSession(&ssl, g_pSslKeys, NULL, 0, certCb, NULL,
				NULL);
		if (rc != MATRIXSSL_REQUEST_SEND) {
			TINYCLR_SSL_PRINTF("New Client Session Failed: %d.  Exiting\n", rc);
			return FALSE;
		}

		g_SSL_Driver.m_sslContextArray[sslCtxIndex].SslContext = ssl;
		g_SSL_Driver.m_sslContextCount++;

		sslContextHandle = sslCtxIndex;
		return TRUE;
	}
	return FALSE;
}

