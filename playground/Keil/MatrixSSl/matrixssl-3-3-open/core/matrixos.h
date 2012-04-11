#ifndef _MATRIX_OS_H_
#define _MATRIX_OS_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


	typedef int32_t int32;
	typedef uint32_t uint32;
	typedef int16_t int16;
	typedef uint16_t uint16;
	#ifdef HAVE_NATIVE_INT64 
	typedef int64_t int64;
	typedef uint64_t uint64;
    #endif           

    	typedef struct {
		long sec;
		long usec;
	} psTime_t;

    typedef uint32 size_t;
    typedef uint64_t time_t;

//#define NULL ((void *)0)
    

extern int32	sslCloseOsdep(void);
 extern int32	sslGetEntropy(unsigned char *bytes, uint32 size);
 extern void psBreak(void);

  #define MAX_MEMORY_USAGE	0
 #define psOpenMalloc(A) 0
 #define psCloseMalloc()



/*
    Use the C runtime memory manager
 */
 #define psMalloc(A, B)		malloc(B)
 #define psCalloc(A, B, C)	calloc(B, C)
 #define psRealloc	realloc
 #define psFree		free
 #define psError(A) 




 #define psMemset	memset
 #define psMemcpy	memcpy
 typedef int32 psPool_t;


 #define psGetEntropy sslGetEntropy
 #define psAssert(C) sslAssert(C);


 #define sslAssert(C)  if (C) ; else \
 	{fprintf(stderr, "%s:%d sslAssert(%s)\n",__FILE__, __LINE__, #C); psBreak(); }

//#define psBreak() {while(1);}

 
//#ifdef LUMINARY_MICRO
/*
    Override the C runtime time() function to allow for a leaner
    implementation.
*/

extern time_t NoOSTime(time_t *pTime);

#define time(x) NoOSTime(x)


//#endif

 /*
 	Defines to make library multithreading safe
 */

 extern void matrixStrDebugMsg(char *message, char *arg);
 extern void matrixIntDebugMsg(char *message, int32 arg);
 extern void matrixPtrDebugMsg(char *message, void *arg);

#ifdef LUMINARY_MICRO
#define printf UARTprintf
#define sslAssert(C)  if (C) ; else \
    {UARTprintf("%s:%d sslAssert(%s)\n",__FILE__, __LINE__, #C); psBreak(); }
#else
 #define sslAssert(C)  if (C) ; else \
 	{fprintf(stderr, "%s:%d sslAssert(%s)\n",__FILE__, __LINE__, #C); psBreak(); }
#endif /* LUMINARY_MICRO */

 #else
 #define matrixStrDebugMsg(x, y)
 #define matrixIntDebugMsg(x, y)
 #define matrixPtrDebugMsg(x, y)



 

 #endif
