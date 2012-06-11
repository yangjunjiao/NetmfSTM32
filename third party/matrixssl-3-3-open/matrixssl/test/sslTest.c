/*
 *	sslTest.c
 *	Release $Name: MATRIXSSL-3-3-0-OPEN $
 *
 *	Self-test of the MatrixSSL handshake protocol and encrypted data exchange.
 *	Each enabled cipher suite is run through all configured SSL handshake paths
 */
/*
 *	Copyright (c) AuthenTec, Inc. 2011-2012
 *	Copyright (c) PeerSec Networks, 2002-2011
 *	All Rights Reserved
 *
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from AuthenTec at
 *	http://www.authentec.com/Products/EmbeddedSecurity/SecurityToolkits.aspx
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#include "matrixssl/matrixsslApi.h"

/*
	This test application can also run in a mode that measures the time of
	SSL connections.  If USE_HIGHRES time is disabled the granularity is
	milliseconds so most non-embedded platforms will report 0 msecs/conn for
	most stats. 

	Standard handshakes and client-auth handshakes (commercial only) are timed
	for each enabled cipher suite. The other handshake types will still run
	but will not be timed
*/
/* #define ENABLE_PERF_TIMING */
#define CONN_ITER			500 /* number of connections per type of hs */
#define APP_DATA_LEN		1024

#ifdef ENABLE_PERF_TIMING
#define testTrace(x)
#define TIME_UNITS "msecs/connection\n"
#else /* !ENABLE_PERF_TIMING */
#define testTrace(x) _psTrace(x)
#endif /* ENABLE_PERF_TIMING */
 
/******************************************************************************/
/*
	Must define in matrixConfig.h:
		USE_SERVER_SIDE_SSL
		USE_CLIENT_SIDE_SSL
		USE_CLIENT_AUTH (commercial only) 
*/
#if !defined(USE_SERVER_SIDE_SSL) || !defined(USE_CLIENT_SIDE_SSL)
#error "Must enable both USE_SERVER_SIDE_SSL and USE_CLIENT_SIDE_SSL to test"
#endif
	
typedef struct {
    ssl_t               *ssl;
    sslKeys_t           *keys;
#ifdef ENABLE_PERF_TIMING
	uint32			runningTime;
#endif	
} sslConn_t;

typedef struct {
    char    name[52];
    int32   cipherId;
    int32   rsa;
    int32   dh;
    int32   psk;
    int32   ecdh;
} testCipherSpec_t;

static	sslSessionId_t	clientSessionId;

/******************************************************************************/
/*
	Key loading.  The header files are a bit easier to work with because
	it is better to get a compile error that a header isn't found rather
	than a run-time error that a .pem file isn't found
*/
#define USE_HEADER_KEYS /* comment out this line to test with .pem files */

#if defined(MATRIX_USE_FILE_SYSTEM) && !defined(USE_HEADER_KEYS)
static char svrKeyFile[] = "../crypto/sampleCerts/privkeySrv.pem";
static char svrCertFile[] = "../crypto/sampleCerts/certSrv.pem";
static char svrCAfile[] = "../crypto/sampleCerts/CAcertSrv.pem";
#endif  /* MATRIX_USE_FILE_SYSTEM && !USE_HEADER_KEYS */

#define USE_SAMPLECERTS
#ifdef USE_SAMPLECERTS

unsigned int serverCert_size=619;
unsigned char serverCert[] = {
0x30,0x82,0x02,0x67,0x30,0x82,0x01,0xd0,0x02,0x01,0x01,0x30,0x0d,0x06,0x09,
0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x05,0x05,0x00,0x30,0x77,0x31,0x0b,
0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x44,0x45,0x31,0x0b,0x30,0x09,
0x06,0x03,0x55,0x04,0x08,0x13,0x02,0x42,0x59,0x31,0x0c,0x30,0x0a,0x06,0x03,
0x55,0x04,0x07,0x13,0x03,0x4d,0x41,0x58,0x31,0x0c,0x30,0x0a,0x06,0x03,0x55,
0x04,0x0a,0x13,0x03,0x4d,0x53,0x4b,0x31,0x0c,0x30,0x0a,0x06,0x03,0x55,0x04,
0x0b,0x13,0x03,0x4d,0x53,0x4b,0x31,0x0c,0x30,0x0a,0x06,0x03,0x55,0x04,0x03,
0x13,0x03,0x4d,0x4b,0x53,0x31,0x23,0x30,0x21,0x06,0x09,0x2a,0x86,0x48,0x86,
0xf7,0x0d,0x01,0x09,0x01,0x16,0x14,0x69,0x6e,0x66,0x6f,0x40,0x6d,0x73,0x6b,
0x2d,0x73,0x6f,0x66,0x74,0x77,0x61,0x72,0x65,0x2e,0x64,0x65,0x30,0x1e,0x17,
0x0d,0x31,0x32,0x30,0x36,0x30,0x34,0x31,0x32,0x35,0x38,0x31,0x36,0x5a,0x17,
0x0d,0x32,0x32,0x30,0x36,0x30,0x32,0x31,0x32,0x35,0x38,0x31,0x36,0x5a,0x30,
0x81,0x80,0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x44,0x45,
0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x08,0x13,0x02,0x42,0x59,0x31,0x0c,
0x30,0x0a,0x06,0x03,0x55,0x04,0x07,0x13,0x03,0x4d,0x41,0x58,0x31,0x0c,0x30,
0x0a,0x06,0x03,0x55,0x04,0x0a,0x13,0x03,0x4d,0x53,0x4b,0x31,0x0c,0x30,0x0a,
0x06,0x03,0x55,0x04,0x0b,0x13,0x03,0x4d,0x53,0x4b,0x31,0x15,0x30,0x13,0x06,
0x03,0x55,0x04,0x03,0x14,0x0c,0x2a,0x2e,0x6e,0x6f,0x2d,0x69,0x70,0x2e,0x69,
0x6e,0x66,0x6f,0x31,0x23,0x30,0x21,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,
0x01,0x09,0x01,0x16,0x14,0x69,0x6e,0x66,0x6f,0x40,0x6d,0x73,0x6b,0x2d,0x73,
0x6f,0x66,0x74,0x77,0x61,0x72,0x65,0x2e,0x64,0x65,0x30,0x81,0x9f,0x30,0x0d,
0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x01,0x05,0x00,0x03,0x81,
0x8d,0x00,0x30,0x81,0x89,0x02,0x81,0x81,0x00,0xb6,0x6a,0x7a,0x47,0xe5,0xcb,
0x77,0x55,0x2e,0x4e,0x27,0x8e,0x29,0xc9,0xd0,0x4f,0x78,0xb9,0x9f,0xad,0x3c,
0x50,0xd3,0x10,0xfe,0x08,0xfa,0x2a,0xa1,0xd7,0xc3,0x9d,0x5b,0x6f,0xa6,0x09,
0x09,0xa3,0xa2,0x73,0xf7,0xe7,0x64,0xbd,0x74,0x51,0xfc,0xe6,0x65,0xb2,0xdd,
0xd9,0x5f,0x43,0x05,0x84,0xfe,0xa6,0x35,0xb5,0x5d,0x8f,0xbe,0x3f,0x1c,0x79,
0xdf,0x03,0xaf,0xd6,0x46,0xa6,0x97,0xfa,0x70,0x2c,0x55,0x39,0x6c,0xf7,0x1b,
0x6f,0x93,0xdb,0x16,0x23,0xdc,0xac,0x77,0xb0,0xea,0xa7,0x3c,0xcf,0xdc,0xae,
0x42,0x0b,0x4c,0x3c,0x11,0xae,0x80,0x4b,0xec,0xbe,0x29,0x55,0xe3,0x48,0x94,
0xea,0x29,0x28,0xb4,0xc5,0xc1,0x86,0x9c,0x84,0x2e,0x84,0x62,0x51,0x3e,0x58,
0x7c,0xe7,0x02,0x03,0x01,0x00,0x01,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,
0xf7,0x0d,0x01,0x01,0x05,0x05,0x00,0x03,0x81,0x81,0x00,0x6f,0x56,0xc3,0x38,
0x2d,0x78,0x6e,0xfd,0xe2,0x1a,0x8e,0x50,0xe5,0x4d,0x85,0x4f,0xf2,0xe2,0xc5,
0x45,0x20,0x90,0x72,0xf4,0x14,0xd8,0x27,0x3f,0x86,0x4b,0x02,0x8d,0xb1,0xfc,
0x32,0x66,0xb0,0x68,0xe1,0x09,0x10,0x97,0x06,0x68,0x1a,0xb9,0x5d,0x50,0x80,
0x17,0x09,0x49,0x4d,0x5a,0xb3,0x6e,0x0f,0xd5,0x56,0xe8,0xfb,0x29,0xef,0x37,
0xda,0xd7,0x06,0xa6,0x7d,0x03,0x5e,0x6a,0xfe,0x63,0xa8,0x91,0xe7,0x41,0x10,
0xb0,0xfa,0x9a,0xb8,0xee,0x66,0x14,0x10,0x83,0x7a,0xee,0x34,0x7d,0x92,0xc9,
0xa6,0x3e,0xf5,0x04,0xea,0x3b,0xa1,0x66,0x6b,0xdc,0x74,0xb8,0xf4,0x00,0xa4,
0x5e,0x5e,0x10,0x43,0x82,0x8d,0x8d,0xd0,0x83,0x0b,0xde,0x0b,0x82,0xee,0xb0,
0x4c,0x81,0xb6,0xf6
};

unsigned int serverKey_size=887;
unsigned char serverKey[] = {
0x2d,0x2d,0x2d,0x2d,0x2d,0x42,0x45,0x47,0x49,0x4e,0x20,0x52,0x53,0x41,0x20,
0x50,0x52,0x49,0x56,0x41,0x54,0x45,0x20,0x4b,0x45,0x59,0x2d,0x2d,0x2d,0x2d,
0x2d,0x0a,0x4d,0x49,0x49,0x43,0x58,0x41,0x49,0x42,0x41,0x41,0x4b,0x42,0x67,
0x51,0x43,0x32,0x61,0x6e,0x70,0x48,0x35,0x63,0x74,0x33,0x56,0x53,0x35,0x4f,
0x4a,0x34,0x34,0x70,0x79,0x64,0x42,0x50,0x65,0x4c,0x6d,0x66,0x72,0x54,0x78,
0x51,0x30,0x78,0x44,0x2b,0x43,0x50,0x6f,0x71,0x6f,0x64,0x66,0x44,0x6e,0x56,
0x74,0x76,0x70,0x67,0x6b,0x4a,0x0a,0x6f,0x36,0x4a,0x7a,0x39,0x2b,0x64,0x6b,
0x76,0x58,0x52,0x52,0x2f,0x4f,0x5a,0x6c,0x73,0x74,0x33,0x5a,0x58,0x30,0x4d,
0x46,0x68,0x50,0x36,0x6d,0x4e,0x62,0x56,0x64,0x6a,0x37,0x34,0x2f,0x48,0x48,
0x6e,0x66,0x41,0x36,0x2f,0x57,0x52,0x71,0x61,0x58,0x2b,0x6e,0x41,0x73,0x56,
0x54,0x6c,0x73,0x39,0x78,0x74,0x76,0x6b,0x39,0x73,0x57,0x0a,0x49,0x39,0x79,
0x73,0x64,0x37,0x44,0x71,0x70,0x7a,0x7a,0x50,0x33,0x4b,0x35,0x43,0x43,0x30,
0x77,0x38,0x45,0x61,0x36,0x41,0x53,0x2b,0x79,0x2b,0x4b,0x56,0x58,0x6a,0x53,
0x4a,0x54,0x71,0x4b,0x53,0x69,0x30,0x78,0x63,0x47,0x47,0x6e,0x49,0x51,0x75,
0x68,0x47,0x4a,0x52,0x50,0x6c,0x68,0x38,0x35,0x77,0x49,0x44,0x41,0x51,0x41,
0x42,0x0a,0x41,0x6f,0x47,0x41,0x48,0x69,0x31,0x37,0x55,0x55,0x32,0x6f,0x33,
0x58,0x4d,0x51,0x47,0x41,0x6e,0x66,0x4d,0x50,0x61,0x63,0x4b,0x61,0x6d,0x56,
0x4f,0x39,0x39,0x53,0x37,0x53,0x51,0x64,0x32,0x59,0x6a,0x75,0x71,0x67,0x4c,
0x67,0x53,0x52,0x4a,0x4e,0x51,0x43,0x6b,0x39,0x73,0x59,0x53,0x4b,0x71,0x5a,
0x6a,0x48,0x68,0x52,0x6a,0x36,0x0a,0x6c,0x31,0x32,0x35,0x56,0x2f,0x58,0x42,
0x55,0x65,0x61,0x36,0x6c,0x62,0x30,0x73,0x79,0x35,0x35,0x7a,0x42,0x4b,0x6c,
0x31,0x46,0x59,0x41,0x68,0x48,0x52,0x32,0x42,0x78,0x6d,0x4f,0x47,0x38,0x4f,
0x4e,0x59,0x69,0x4c,0x6d,0x30,0x4b,0x6d,0x32,0x54,0x66,0x34,0x2b,0x6b,0x6b,
0x64,0x50,0x62,0x5a,0x56,0x4c,0x43,0x50,0x30,0x69,0x55,0x0a,0x66,0x66,0x6e,
0x31,0x65,0x48,0x71,0x46,0x41,0x43,0x63,0x55,0x6d,0x61,0x69,0x47,0x6c,0x39,
0x48,0x6f,0x6e,0x6c,0x4a,0x51,0x46,0x54,0x79,0x2f,0x76,0x32,0x69,0x38,0x5a,
0x4f,0x6f,0x4b,0x39,0x33,0x55,0x56,0x69,0x6c,0x2f,0x66,0x35,0x6b,0x45,0x43,
0x51,0x51,0x44,0x6d,0x55,0x64,0x59,0x33,0x50,0x6d,0x70,0x32,0x4a,0x66,0x34,
0x32,0x0a,0x72,0x49,0x42,0x4c,0x30,0x4a,0x2f,0x71,0x6f,0x7a,0x6e,0x63,0x50,
0x54,0x4e,0x74,0x71,0x39,0x55,0x30,0x63,0x41,0x39,0x48,0x65,0x55,0x63,0x66,
0x30,0x6f,0x68,0x2f,0x61,0x66,0x6f,0x5a,0x4f,0x36,0x6a,0x72,0x43,0x63,0x70,
0x6c,0x41,0x53,0x6c,0x57,0x64,0x2b,0x69,0x54,0x5a,0x79,0x65,0x58,0x38,0x65,
0x69,0x67,0x52,0x6a,0x61,0x42,0x0a,0x6e,0x58,0x4d,0x5a,0x65,0x77,0x6c,0x78,
0x41,0x6b,0x45,0x41,0x79,0x73,0x46,0x4c,0x46,0x67,0x73,0x37,0x50,0x4e,0x6c,
0x33,0x53,0x4e,0x72,0x44,0x51,0x2b,0x59,0x4a,0x42,0x32,0x73,0x61,0x48,0x4a,
0x50,0x37,0x4b,0x64,0x77,0x77,0x49,0x6c,0x6b,0x35,0x4d,0x43,0x72,0x6b,0x30,
0x44,0x75,0x73,0x55,0x76,0x47,0x37,0x4d,0x53,0x57,0x45,0x0a,0x77,0x65,0x7a,
0x6f,0x68,0x41,0x79,0x51,0x49,0x58,0x67,0x63,0x68,0x37,0x55,0x55,0x70,0x77,
0x6e,0x2b,0x64,0x41,0x45,0x62,0x38,0x6d,0x66,0x78,0x74,0x56,0x7a,0x2f,0x31,
0x77,0x4a,0x41,0x65,0x74,0x79,0x4d,0x2b,0x55,0x74,0x43,0x58,0x32,0x52,0x46,
0x42,0x38,0x56,0x64,0x61,0x78,0x51,0x50,0x4f,0x6f,0x74,0x34,0x41,0x68,0x52,
0x6a,0x0a,0x71,0x73,0x31,0x50,0x48,0x6e,0x6e,0x6f,0x72,0x4b,0x66,0x31,0x57,
0x50,0x52,0x6c,0x2f,0x42,0x48,0x38,0x61,0x42,0x6c,0x37,0x4d,0x6d,0x59,0x32,
0x6f,0x55,0x72,0x4e,0x55,0x6d,0x4d,0x45,0x6e,0x68,0x30,0x59,0x38,0x65,0x55,
0x4a,0x77,0x76,0x57,0x78,0x66,0x64,0x4d,0x50,0x34,0x7a,0x66,0x50,0x41,0x51,
0x4a,0x42,0x41,0x4a,0x4d,0x36,0x0a,0x65,0x76,0x43,0x64,0x35,0x54,0x30,0x52,
0x57,0x4b,0x6f,0x42,0x6d,0x4d,0x73,0x6b,0x4c,0x69,0x68,0x42,0x34,0x47,0x73,
0x69,0x46,0x62,0x37,0x57,0x30,0x65,0x6b,0x50,0x53,0x78,0x53,0x73,0x41,0x51,
0x30,0x6f,0x56,0x38,0x43,0x2f,0x49,0x63,0x50,0x56,0x54,0x36,0x71,0x57,0x41,
0x6c,0x31,0x6f,0x31,0x66,0x78,0x48,0x77,0x4d,0x66,0x57,0x0a,0x65,0x52,0x47,
0x32,0x6b,0x78,0x76,0x66,0x64,0x6c,0x43,0x32,0x6f,0x2f,0x43,0x6a,0x67,0x47,
0x38,0x43,0x51,0x46,0x76,0x6b,0x6b,0x58,0x53,0x61,0x58,0x48,0x4f,0x6e,0x66,
0x33,0x30,0x68,0x71,0x6a,0x41,0x72,0x41,0x54,0x41,0x72,0x53,0x66,0x68,0x66,
0x2f,0x6a,0x46,0x61,0x73,0x66,0x35,0x36,0x58,0x72,0x72,0x69,0x56,0x49,0x31,
0x71,0x0a,0x54,0x36,0x38,0x6f,0x48,0x58,0x33,0x71,0x30,0x37,0x4f,0x4a,0x65,
0x53,0x37,0x4c,0x36,0x4e,0x67,0x69,0x56,0x43,0x49,0x75,0x35,0x6f,0x43,0x4b,
0x57,0x76,0x53,0x77,0x33,0x2b,0x4a,0x55,0x57,0x4a,0x6b,0x72,0x44,0x36,0x55,
0x3d,0x0a,0x2d,0x2d,0x2d,0x2d,0x2d,0x45,0x4e,0x44,0x20,0x52,0x53,0x41,0x20,
0x50,0x52,0x49,0x56,0x41,0x54,0x45,0x20,0x4b,0x45,0x59,0x2d,0x2d,0x2d,0x2d,
0x2d,0x0a
};


#endif
					
#ifdef USE_HEADER_KEYS
#include "sampleCerts/privkeySrv.h"
#include "sampleCerts/certSrv.h"
#include "sampleCerts/CAcertSrv.h"
#endif /* USE_HEADER_KEYS */

static void freeSessionAndConnection(sslConn_t *cpp);

static int32 initializeServer(sslConn_t *svrConn, testCipherSpec_t cipher);
static int32 initializeClient(sslConn_t *clnConn, testCipherSpec_t cipher,
				 sslSessionId_t *sid);

static int32 initializeHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
								 testCipherSpec_t cipherSuite,
								 sslSessionId_t *sid);
								 
static int32 initializeResumedHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
								testCipherSpec_t cipherSuite);
								
#ifdef SSL_REHANDSHAKES_ENABLED								
static int32 initializeReHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
								int32 cipherSuite);

static int32 initializeResumedReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);
static int32 initializeServerInitiatedReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);
static int32 initializeServerInitiatedResumedReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);	
static int32 initializeUpgradeCertCbackReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);
static int32 initializeUpgradeKeysReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);
static int32 initializeChangeCipherReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite);											
#endif /* SSL_REHANDSHAKES_ENABLED */
															
								
static int32 performHandshake(sslConn_t *sendingSide, sslConn_t *receivingSide);
static int32 exchangeAppData(sslConn_t *sendingSide, sslConn_t *receivingSide);

/*
	Client-authentication.  Callback that is registered to receive client
	certificate information for custom validation
*/
static int32 clnCertChecker(ssl_t *ssl, psX509Cert_t *cert, int32 alert);

#ifdef SSL_REHANDSHAKES_ENABLED 
static int32 clnCertCheckerUpdate(ssl_t *ssl, psX509Cert_t *cert, int32 alert);
#endif


static testCipherSpec_t	ciphers[] = {

#ifdef USE_SSL_RSA_WITH_NULL_SHA
	{"SSL_RSA_WITH_NULL_SHA",
		SSL_RSA_WITH_NULL_SHA,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif

#ifdef USE_SSL_RSA_WITH_NULL_MD5	
	{"SSL_RSA_WITH_NULL_MD5",
		SSL_RSA_WITH_NULL_MD5,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif

#ifdef USE_TLS_RSA_WITH_AES_256_CBC_SHA
	{"TLS_RSA_WITH_AES_256_CBC_SHA",
		TLS_RSA_WITH_AES_256_CBC_SHA,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif


#ifdef USE_TLS_RSA_WITH_AES_128_CBC_SHA
	{"TLS_RSA_WITH_AES_128_CBC_SHA",
		TLS_RSA_WITH_AES_128_CBC_SHA,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif

#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	{"SSL_RSA_WITH_3DES_EDE_CBC_SHA",
		SSL_RSA_WITH_3DES_EDE_CBC_SHA,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_SHA	
	{"SSL_RSA_WITH_RC4_128_SHA",
		SSL_RSA_WITH_RC4_128_SHA,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif

#ifdef USE_SSL_RSA_WITH_RC4_128_MD5	
	{"SSL_RSA_WITH_RC4_128_MD5",
		SSL_RSA_WITH_RC4_128_MD5,
		1,			/* RSA keys? */
		0,			/* DH keys */
		0,			/* PSK */
		0			/* ECDH keys */
	},
#endif
	
	
	{"NULL", -1, 0, 0, 0, 0} /* must be last */
};

/******************************************************************************/
/*
	This test application will exercise the SSL/TLS handshake and app
	data exchange for every eligible cipher.  
*/

int main(int argc, char **argv)
{
	int32			id;
	sslConn_t		*svrConn, *clnConn;
#ifdef ENABLE_PERF_TIMING
	int32			perfIter;
	uint32			clnTime, svrTime;
#endif /* ENABLE_PERF_TIMING */

	if (matrixSslOpen() < 0) {
		fprintf(stderr, "matrixSslOpen failed, exiting...");
	}

	svrConn = psMalloc(PEERSEC_NO_POOL, sizeof(sslConn_t));
	clnConn = psMalloc(PEERSEC_NO_POOL, sizeof(sslConn_t));
	memset(svrConn, 0, sizeof(sslConn_t));
	memset(clnConn, 0, sizeof(sslConn_t));
	
	for (id = 0; ciphers[id].cipherId > 0; id++) {
		matrixSslInitSessionId(clientSessionId);
		_psTraceStr("Testing %s suite\n", ciphers[id].name);
/*
		Standard Handshake
*/
		_psTrace("	Standard handshake test\n");
#ifdef ENABLE_PERF_TIMING
/*
		Each matrixSsl call in the handshake is wrapped by a timer.  Data
		exchange is NOT included in the timer
*/
		clnTime = svrTime = 0;
		_psTraceInt("		%d connections\n", (int32)CONN_ITER);
		for (perfIter = 0; perfIter < CONN_ITER; perfIter++) {
#endif /* ENABLE_PERF_TIMING */		
		if (initializeHandshake(clnConn, svrConn, ciphers[id],
				&clientSessionId) < 0) {
			_psTrace("		FAILED: initializing Standard handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Standard handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Standard handshake");	
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange srv application data\n");
			} else {
				testTrace("\n");
			}
		}
#ifdef ENABLE_PERF_TIMING
		svrTime += svrConn->runningTime;
		clnTime += clnConn->runningTime;
		/* Have to reset conn for full handshake... except last time through */
		if (perfIter + 1 != CONN_ITER) {
			matrixSslDeleteSession(clnConn->ssl);
			matrixSslDeleteSession(svrConn->ssl);
			matrixSslInitSessionId(clientSessionId);
		}
		} /* iteration loop close */
		_psTraceInt("		CLIENT:	%d " TIME_UNITS, (int32)clnTime/CONN_ITER);
		_psTraceInt("		SERVER:	%d " TIME_UNITS, (int32)svrTime/CONN_ITER);
		_psTrace("\n");
#endif /* ENABLE_PERF_TIMING */
		
#ifdef SSL_REHANDSHAKES_ENABLED	
/*
		 Re-Handshake (full handshake over existing connection)
*/		
		testTrace("	Re-handshake test (client-initiated)\n");		
		if (initializeReHandshake(clnConn, svrConn, ciphers[id].cipherId) < 0) {
			_psTrace("		FAILED: initializing Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Re-handshake");			
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}	
#else
		_psTrace("	Re-handshake tests are disabled (ENABLE_SECURE_REHANDSHAKES)\n");
#endif
				
/*
		Resumed handshake (fast handshake over new connection)
*/				
		testTrace("	Resumed handshake test (new connection)\n");		
		if (initializeResumedHandshake(clnConn, svrConn,
				ciphers[id]) < 0) {
			_psTrace("		FAILED: initializing Resumed handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Resumed handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Resumed handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}	
		
#ifdef SSL_REHANDSHAKES_ENABLED		
/*
		 Re-handshake initiated by server (full handshake over existing conn)
*/			
		testTrace("	Re-handshake test (server initiated)\n");
		if (initializeServerInitiatedReHandshake(clnConn, svrConn,
									   ciphers[id].cipherId) < 0) {
			_psTrace("		FAILED: initializing Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(svrConn, clnConn) < 0) {
			_psTrace("		FAILED: Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Re-handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}	
	
		/* Testing 5 more re-handshake paths.  Add some credits */
		matrixSslAddRehandshakeCredits(svrConn->ssl, 5);
/*
		Resumed re-handshake (fast handshake over existing connection)
*/				
		testTrace("	Resumed Re-handshake test (client initiated)\n");
		if (initializeResumedReHandshake(clnConn, svrConn,
				 ciphers[id].cipherId) < 0) {
				_psTrace("		FAILED: initializing Resumed Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Resumed Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Resumed Re-handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}
		
/*
		 Resumed re-handshake initiated by server (fast handshake over conn)
*/		
		testTrace("	Resumed Re-handshake test (server initiated)\n");
		if (initializeServerInitiatedResumedReHandshake(clnConn, svrConn,
									   ciphers[id].cipherId) < 0) {
				_psTrace("		FAILED: initializing Resumed Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(svrConn, clnConn) < 0) {
			_psTrace("		FAILED: Resumed Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Resumed Re-handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}		
/*
		Re-handshaking with "upgraded" parameters
*/
		testTrace("	Change cert callback Re-handshake test\n");
		if (initializeUpgradeCertCbackReHandshake(clnConn, svrConn,
									   ciphers[id].cipherId) < 0) {
				_psTrace("		FAILED: init upgrade certCback Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Upgrade cert callback Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Upgrade cert callback Re-handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}		
/*
		Upgraded keys
*/
		testTrace("	Change keys Re-handshake test\n");
		if (initializeUpgradeKeysReHandshake(clnConn, svrConn,
									   ciphers[id].cipherId) < 0) {
				_psTrace("		FAILED: init upgrade keys Re-handshake\n");
			goto LBL_FREE;
		}
		if (performHandshake(clnConn, svrConn) < 0) {
			_psTrace("		FAILED: Upgrade keys Re-handshake\n");
			goto LBL_FREE;
		} else {
			testTrace("		PASSED: Upgrade keys Re-handshake");
			if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
				_psTrace(" but FAILED to exchange application data\n");
			} else {
				testTrace("\n");
			}
		}
/*
		Change cipher spec test.  Changing to a hardcoded RSA suite so this
		will not work on suites that don't have RSA material loaded
*/
		if (ciphers[id].rsa == 1 && ciphers[id].ecdh == 0) {
			testTrace("	Change cipher suite Re-handshake test\n");
			if (initializeChangeCipherReHandshake(clnConn, svrConn,
									   ciphers[id].cipherId) < 0) {
					_psTrace("		FAILED: init change cipher Re-handshake\n");
				goto LBL_FREE;
			}
			if (performHandshake(clnConn, svrConn) < 0) {
				_psTrace("		FAILED: Change cipher suite Re-handshake\n");
				goto LBL_FREE;
			} else {
				testTrace("		PASSED: Change cipher suite Re-handshake");
				if (exchangeAppData(clnConn, svrConn) < 0 ||
					exchangeAppData(svrConn, clnConn) < 0) {
					_psTrace(" but FAILED to exchange application data\n");
				} else {
					testTrace("\n");
				}
			}
		}
#endif /* !SSL_REHANDSHAKES_ENABLED */


LBL_FREE:
		freeSessionAndConnection(svrConn);
		freeSessionAndConnection(clnConn);
	}
	psFree(svrConn);
	psFree(clnConn);
	matrixSslClose();

#ifdef WIN32
	_psTrace("Press any key to close");
	getchar();
#endif

	return PS_SUCCESS;	
}

static int32 initializeHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
							testCipherSpec_t cipherSuite, sslSessionId_t *sid)
{
	int32	rc;
	
	rc = initializeServer(svrConn, cipherSuite);
	rc = initializeClient(clnConn, cipherSuite, sid);

	return rc;
}

#ifdef SSL_REHANDSHAKES_ENABLED
static int32 initializeReHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
								   int32 cipherSuite)
{
	return matrixSslEncodeRehandshake(clnConn->ssl, NULL, NULL,
		SSL_OPTION_FULL_HANDSHAKE, cipherSuite);
}

static int32 initializeServerInitiatedReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite)
{
	return matrixSslEncodeRehandshake(svrConn->ssl, NULL, NULL,
		SSL_OPTION_FULL_HANDSHAKE, cipherSuite);
}

static int32 initializeServerInitiatedResumedReHandshake(sslConn_t *clnConn,
								sslConn_t *svrConn, int32 cipherSuite)
{
	return matrixSslEncodeRehandshake(svrConn->ssl, NULL, NULL, 0, cipherSuite);

}

static int32 initializeResumedReHandshake(sslConn_t *clnConn,
							sslConn_t *svrConn, int32 cipherSuite)
{
	return matrixSslEncodeRehandshake(clnConn->ssl, NULL, NULL, 0, cipherSuite);
}

static int32 initializeUpgradeCertCbackReHandshake(sslConn_t *clnConn,
							sslConn_t *svrConn, int32 cipherSuite)
{
	return matrixSslEncodeRehandshake(clnConn->ssl, NULL, clnCertCheckerUpdate,
							0, cipherSuite);
}

static int32 initializeUpgradeKeysReHandshake(sslConn_t *clnConn,
							sslConn_t *svrConn, int32 cipherSuite)
{
/*
	Not really changing the keys but this still tests that passing a
	valid arg will force a full handshake
*/
	return matrixSslEncodeRehandshake(clnConn->ssl, clnConn->ssl->keys, NULL,
							0, cipherSuite);
}

static int32 initializeChangeCipherReHandshake(sslConn_t *clnConn,
							sslConn_t *svrConn, int32 cipherSuite)
{
/*
	Just picking the most common suite
*/
#ifdef USE_SSL_RSA_WITH_3DES_EDE_CBC_SHA
	return matrixSslEncodeRehandshake(clnConn->ssl, NULL, NULL,
					0, SSL_RSA_WITH_3DES_EDE_CBC_SHA);
#else
	return matrixSslEncodeRehandshake(clnConn->ssl, NULL, NULL, 0, 0);
#endif
}

#endif /* SSL_REHANDSHAKES_ENABLED */

static int32 initializeResumedHandshake(sslConn_t *clnConn, sslConn_t *svrConn,
										testCipherSpec_t cipherSuite)
{
	sslSessionId_t	*sessionId;
#ifdef ENABLE_PERF_TIMING
	psTime_t		start, end;
#endif /* ENABLE_PERF_TIMING */	
	
	sessionId = clnConn->ssl->sid;
	
	matrixSslDeleteSession(clnConn->ssl);

#ifdef ENABLE_PERF_TIMING
	clnConn->runningTime = 0;
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	
	if (matrixSslNewClientSession(&clnConn->ssl, clnConn->keys, sessionId,
			cipherSuite.cipherId, clnCertChecker, NULL, NULL) < 0) {
        return PS_FAILURE;
    }
#ifdef ENABLE_PERF_TIMING
	psGetTime(&end);
	clnConn->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */
	
	matrixSslDeleteSession(svrConn->ssl);
#ifdef ENABLE_PERF_TIMING
	svrConn->runningTime = 0;
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */		
	if (matrixSslNewServerSession(&svrConn->ssl, svrConn->keys, NULL) < 0) {
        return PS_FAILURE;
    }
#ifdef ENABLE_PERF_TIMING
	psGetTime(&end);
	svrConn->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */	
	return PS_SUCCESS;
}



/*
	Recursive handshake
*/
static int32 performHandshake(sslConn_t *sendingSide, sslConn_t *receivingSide)
{
	unsigned char	*inbuf, *outbuf, *plaintextBuf;
	int32			inbufLen, outbufLen, rc, sdrc, dataSent;
	uint32			ptLen;
#ifdef ENABLE_PERF_TIMING	
	psTime_t		start, end;
#endif /* ENABLE_PERF_TIMING */	
	
/*
	Sending side will have outdata ready
*/
#ifdef ENABLE_PERF_TIMING
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	
	outbufLen = matrixSslGetOutdata(sendingSide->ssl, &outbuf);
#ifdef ENABLE_PERF_TIMING	
	psGetTime(&end);
	sendingSide->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */	
	
/*
	Receiving side must ask for storage space to receive data into
*/
#ifdef ENABLE_PERF_TIMING
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	
	inbufLen = matrixSslGetReadbuf(receivingSide->ssl, &inbuf);
#ifdef ENABLE_PERF_TIMING	
	psGetTime(&end);
	receivingSide->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */	
	
/*
	The indata is the outdata from the sending side.  copy it over
*/
	dataSent = min(outbufLen, inbufLen);
	memcpy(inbuf, outbuf, dataSent);
	
/*
	Now update the sending side that data has been "sent"
*/
#ifdef ENABLE_PERF_TIMING
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	
	sdrc = matrixSslSentData(sendingSide->ssl, dataSent);
#ifdef ENABLE_PERF_TIMING	
	psGetTime(&end);
	sendingSide->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */

/*
	Received data
*/
#ifdef ENABLE_PERF_TIMING
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	
	rc = matrixSslReceivedData(receivingSide->ssl, dataSent, &plaintextBuf,
		&ptLen);
#ifdef ENABLE_PERF_TIMING	
	psGetTime(&end);
	receivingSide->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */	
		
	if (rc == MATRIXSSL_REQUEST_SEND) {
/*
		Success case.  Switch roles and continue
*/
		return performHandshake(receivingSide, sendingSide);
		
	} else if (rc == MATRIXSSL_REQUEST_RECV) {
/*
		This pass didn't take care of it all.  Don't switch roles and
		try again
*/
		return performHandshake(sendingSide, receivingSide);
		
	} else if (rc == MATRIXSSL_HANDSHAKE_COMPLETE) {
		return PS_SUCCESS;

	} else if (rc == MATRIXSSL_RECEIVED_ALERT) {
/*
		Just continue if warning level alert
*/
		if (plaintextBuf[0] == SSL_ALERT_LEVEL_WARNING) {
			if (matrixSslProcessedData(receivingSide->ssl, &plaintextBuf,
					&ptLen) != 0) {
				return PS_FAILURE;	
			}
			return performHandshake(sendingSide, receivingSide);
		} else {
			return PS_FAILURE;
		}
	
	} else {
		printf("Unexpected error in performHandshake: %d\n", rc);
		return PS_FAILURE;
	}
	return PS_FAILURE; /* can't get here */
}

/*
	return 0 on successful encryption/decryption communication
	return -1 on failed comm
*/
static int32 exchangeAppData(sslConn_t *sendingSide, sslConn_t *receivingSide)
{
	int32			writeBufLen, inBufLen, dataSent, rc;
	uint32			ptLen, totalProcessed, requestedLen;
	unsigned char	*writeBuf, *inBuf, *plaintextBuf;

/*
	Client sends the data
*/
	requestedLen = APP_DATA_LEN;
	writeBufLen = matrixSslGetWritebuf(sendingSide->ssl, &writeBuf,
		requestedLen);
		
	psAssert(writeBufLen >= (int32)requestedLen);	
	memset(writeBuf, 4, (size_t)requestedLen);
	
/*
	EncodeWriteBuf does not return the buffer.
*/	
	writeBufLen = matrixSslEncodeWritebuf(sendingSide->ssl, requestedLen);
		
	totalProcessed = 0;	
	
SEND_MORE:		
	writeBufLen = matrixSslGetOutdata(sendingSide->ssl, &writeBuf);	

/*
	Receiving side must ask for storage space to receive data into.
	
	A good optimization of the buffer management can be seen here if a
	second pass was required:  the inBufLen should exactly match the
	writeBufLen because when matrixSslReceivedData was called below the
	record length was parsed off and the buffer was reallocated to the
	exact necessary length
*/
	inBufLen = matrixSslGetReadbuf(receivingSide->ssl, &inBuf);
	
	dataSent = min(writeBufLen, inBufLen);
	memcpy(inBuf, writeBuf, dataSent);

/*
	Now update the sending side that data has been "sent"
*/
	matrixSslSentData(sendingSide->ssl, dataSent);
		
/*
	Received data
*/
	rc = matrixSslReceivedData(receivingSide->ssl, dataSent, &plaintextBuf,
		&ptLen);
		
	if (rc == MATRIXSSL_REQUEST_RECV) {
		goto SEND_MORE;
	} else if (rc == MATRIXSSL_APP_DATA) {
		if (matrixSslProcessedData(receivingSide->ssl, &plaintextBuf,
				&ptLen) != 0) {
			return PS_FAILURE;
		}
	} else {
		printf("Unexpected error in exchangeAppData: %d\n", rc);
		return PS_FAILURE;
	}
	
	return PS_SUCCESS;
}


static int32 initializeServer(sslConn_t *conn, testCipherSpec_t cipherSuite)
{
	sslKeys_t	*keys = NULL;
	ssl_t		*ssl = NULL;
#ifdef ENABLE_PERF_TIMING
	psTime_t	start, end;
#endif /* ENABLE_PERF_TIMING */	

	if (conn->keys == NULL) {
		if (matrixSslNewKeys(&keys) < PS_SUCCESS) {
			return PS_MEM_FAIL;
		}
		conn->keys = keys;




		if (cipherSuite.rsa && !cipherSuite.ecdh) {
#if defined(MATRIX_USE_FILE_SYSTEM) && !defined(USE_HEADER_KEYS)	
			if (matrixSslLoadRsaKeys(keys, svrCertFile, svrKeyFile, NULL, NULL)
					< 0) {
				return PS_FAILURE;
			}
#endif /* MATRIX_USE_FILE_SYSTEM && !USE_HEADER_KEYS */

#ifdef USE_SAMPLECERTS
            if (matrixSslLoadRsaKeysMem(keys, serverCert, sizeof(serverCert),
						serverKey, sizeof(serverKey), NULL, 0) < 0) {
				return PS_FAILURE;
			}
#else

#ifdef USE_HEADER_KEYS
			if (matrixSslLoadRsaKeysMem(keys, certSrvBuf, sizeof(certSrvBuf),
						privkeySrvBuf, sizeof(privkeySrvBuf), NULL, 0) < 0) {
				return PS_FAILURE;
			}
#endif /* USE_HEADER_KEYS */
#endif //USE_SAMPLECERTS
		}
		
	}
#ifdef ENABLE_PERF_TIMING
	conn->runningTime = 0;
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */
/*
	Create a new SSL session for the new socket and register the
	user certificate validator. No client auth first time through
*/
    if (matrixSslNewServerSession(&ssl, conn->keys, NULL) < 0) {
        return PS_FAILURE;
    }
	
#ifdef ENABLE_PERF_TIMING
	psGetTime(&end);
	conn->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */
	conn->ssl = ssl;

	return PS_SUCCESS;
}


static int32 initializeClient(sslConn_t *conn, testCipherSpec_t cipherSuite,
				 sslSessionId_t *sid)
{
	ssl_t		*ssl;
	sslKeys_t	*keys;
#ifdef ENABLE_PERF_TIMING
	psTime_t	start, end;
#endif /* ENABLE_PERF_TIMING */

	if (conn->keys == NULL) {
		if (matrixSslNewKeys(&keys) < PS_SUCCESS) {
			return PS_MEM_FAIL;
		}
		conn->keys = keys;
		

		if (cipherSuite.rsa && !cipherSuite.ecdh) {
#if defined(MATRIX_USE_FILE_SYSTEM) && !defined(USE_HEADER_KEYS)
			if (matrixSslLoadRsaKeys(keys, NULL, NULL, NULL, svrCAfile) < 0) {
				return PS_FAILURE;
			}
#endif /* MATRIX_USE_FILE_SYSTEM && !USE_HEADER_KEYS */

#ifdef USE_HEADER_KEYS
			if (matrixSslLoadRsaKeysMem(keys, NULL, 0, NULL, 0, CAcertSrvBuf,
						sizeof(CAcertSrvBuf)) < 0) {
				return PS_FAILURE;
			}
#endif /* USE_HEADER_KEYS */
		}
	
	}
	
	conn->ssl = NULL;
#ifdef ENABLE_PERF_TIMING
	conn->runningTime = 0;
	psGetTime(&start);
#endif /* ENABLE_PERF_TIMING */	

    if (matrixSslNewClientSession(&ssl, conn->keys, sid, cipherSuite.cipherId,
			clnCertChecker, NULL, NULL) < 0) {
        return PS_FAILURE;
    }
	
#ifdef ENABLE_PERF_TIMING
	psGetTime(&end);
	conn->runningTime += psDiffMsecs(start, end);
#endif /* ENABLE_PERF_TIMING */

	conn->ssl = ssl;
	
	return PS_SUCCESS;
}

/******************************************************************************/
/*
	Delete session and connection
 */
static void freeSessionAndConnection(sslConn_t *conn)
{
	if (conn->ssl != NULL) {
		matrixSslDeleteSession(conn->ssl);
	}
   	matrixSslDeleteKeys(conn->keys);
	conn->ssl = NULL;
	conn->keys = NULL;
}

static int32 clnCertChecker(ssl_t *ssl, psX509Cert_t *cert, int32 alert)
{
	return alert;
}	

#ifdef SSL_REHANDSHAKES_ENABLED
static int32 clnCertCheckerUpdate(ssl_t *ssl, psX509Cert_t *cert, int32 alert)
{
	return alert;
}	
#endif /* SSL_REHANDSHAKES_ENABLED */


/******************************************************************************/



