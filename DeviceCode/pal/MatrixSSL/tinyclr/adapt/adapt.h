 #ifndef _ADAPT_H_
 #define _ADAPT_H_




#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



typedef  ssl_t SSL;
	/*typedef int32_t i32_t;
	typedef uint32_t u32_t;
	typedef int16_t i16_t;
	typedef uint16_t u16_t;
    typedef uint8_t u8_t;
    typedef int8_t i8_t;
     typedef int8_t s8_t;*/
	//typedef uint32_t err_t;


#define TINYCLR_SSL_STRCAT              strcat
#define TINYCLR_SSL_STRCPY(a,b)         hal_strcpy_s(a,hal_strlen_s(b)+1,b)
#define TINYCLR_SSL_STRLEN              hal_strlen_s
#define TINYCLR_SSL_STRNCPY(a,b,c)      hal_strncpy_s(a,c+1,b,c)
#define TINYCLR_SSL_STRNCMP             OPENSSL_strncasecmp
#define TINYCLR_SSL_STRCMP              tinyclr_strcmp
#define TINYCLR_SSL_STRNCASECMP         OPENSSL_strncasecmp
#define TINYCLR_SSL_STRCASECMP          OPENSSL_strcasecmp
#define TINYCLR_SSL_FPRINTF             hal_fprintf_ssl
#define TINYCLR_SSL_SNPRINTF            hal_snprintf
#define TINYCLR_SSL_PRINTF              hal_printf
#define TINYCLR_SSL_PERROR              TINYCLR_SSL_PRINTF

#define TINYCLR_SSL_MEMCPY              memcpy
#define TINYCLR_SSL_MEMSET              memset
#define TINYCLR_SSL_MEMCMP              memcmp
#define TINYCLR_SSL_MEMCHR              memchr
#define TINYCLR_SSL_MEMMOVE             memmove

#define TINYCLR_SSL_TIME                tinyclr_time
#define TINYCLR_SSL_LOCALTIME           tinyclr_localtime
#define TINYCLR_SSL_GMTIME              tinyclr_gmtime
#define TINYCLR_SSL_MKTIME              tinyclr_mktime

#define TINYCLR_SSL_TOUPPER             tinyclr_ssl_toupper
#define TINYCLR_SSL_TOLOWER             tinyclr_ssl_tolower
#define TINYCLR_SSL_LSEEK               tinyclr_ssl_lseek
#define TINYCLR_SSL_CHMOD               tinyclr_ssl_chmod
#define TINYCLR_SSL_FWRITE              tinyclr_ssl_fwrite
#define TINYCLR_SSL_STAT                tinyclr_ssl_stat
#define TINYCLR_SSL_GETTIMEOFDAY        tinyclr_ssl_gettimeofday
#define TINYCLR_SSL_GETPID              tinyclr_ssl_getpid
#define OPENSSL_TYPE__FILE_STDERR       &SSL_STDERR //MS: TODOsomething else later?
#define OPENSSL_TYPE__FILE_STDOUT       &SSL_STDERR //MS: TODOsomething else later? 
#define OPENSSL_TYPE__FILE_STDIN        &SSL_STDIN //MS: TODOsomething else later?
#define TINYCLR_SSL_FILE                OPENSSL_TYPE__FILE

#define TINYCLR_SSL_ASSERT(x)           ASSERT(x)

#define TINYCLR_SSL_QSORT               qsort
#define TINYCLR_SSL_EXIT(x)             if (x==0) return else TINYCLR_SSL_ASSERT(x)
#define TINYCLR_SSL_ABORT()             {TINYCLR_SSL_PRINTF("%s:%d",__FILE__,__LINE__);TINYCLR_SSL_ASSERT(1);}
#define TINYCLR_SSL_MALLOC              private_malloc 
#define TINYCLR_SSL_FREE                private_free
#define TINYCLR_SSL_REALLOC             private_realloc
#define TINYCLR_SSL_GETENV(x)           (NULL)

 #endif
