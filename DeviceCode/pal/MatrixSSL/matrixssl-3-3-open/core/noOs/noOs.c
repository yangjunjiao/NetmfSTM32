
#include "../matrixos.h"

//*****************************************************************************
//
// Multi-threading cannot be supported in the absence of an OS so catch
// this early and generate a compiler error.
//
//*****************************************************************************
#ifdef USE_MULTITHREADING
#error Multi-threading support not available without an OS.
#endif

//*****************************************************************************
//
// Set aside sufficient memory to act as the runtime heap for the SSL library.
// Note that we declare this in terms of "unsigned longs" to ensure that the
// heap is aligned on a word boundary. This is vital!
//
//*****************************************************************************

//*****************************************************************************
//
// Globals related to the hardware timer we use to track system time.
//
//*****************************************************************************

static volatile unsigned long g_ulSeconds = 0;
static unsigned long g_ulClockRate;
//static tBoolean g_bSslTimerInitialized = false;

//*****************************************************************************
//
// The number of elapsed seconds which, if exceeded, will cause overflow in
// the seconds-to-milliseconds calculation in sslDiffMsecs.
//
//*****************************************************************************
#define MAX_OVERFLOW_FREE_SECONDS (0x7FFFFFFF / 1000)

//*****************************************************************************
//
// The bit mask of the polynomial bits used in the random number sequence.
//
//*****************************************************************************
#define LSFR_PATTERN        (0x10350103)

//*****************************************************************************
//
// The maximum possible random number provided by the rand() function.
//
//*****************************************************************************
#define MAX_RAND            (0xffffffff)

//*****************************************************************************
//
// The current value of the random number lsfr shift register.
//
//*****************************************************************************
static unsigned long g_ulRandRegister;

//*****************************************************************************
//! Used to seed the pseudo random number generator.
//!
//! \param ulSeed
//!     The seed value for the random number generator.
//!
//*****************************************************************************
static void SeedRandom(unsigned long ulSeed) {
	g_ulRandRegister = ulSeed;
}

//*****************************************************************************
//! Used to generate a pseudo random number.
//!
//! \return
//!     Returns a value between 0 and MAX_RAND.
//!
//*****************************************************************************
static unsigned long GetRandom(void) {
	if (g_ulRandRegister & 1) {
		g_ulRandRegister = (g_ulRandRegister >> 1) | 0x80000000;
		g_ulRandRegister ^= LSFR_PATTERN;
	} else {
		g_ulRandRegister = g_ulRandRegister >> 1;
	}
	return (g_ulRandRegister);
}

//*****************************************************************************
//
//! Handles the interrupt from the SSL library's hardware timer.
//!
//! \param None.
//!
//! This function increments the elapsed seconds counter used to provide a
//! time reference to the SSL library.
//!
//! \Returns None.
//
//*****************************************************************************
void SslTimerIntHandler(void) {
	//
	// Clear all pending interrupts from our timer
	//
	//TimerIntClear(g_ulSslTimerBase, TimerIntStatus(g_ulSslTimerBase, true));

	//
	// Increment our elapsed second counter.
	//
	//g_ulSeconds+;
}

//*****************************************************************************
//
//! C runtime abort function.
//!
//! \param None.
//! parameter is ignored.
//!
//! This function is called in various C runtime library error conditions.
//! Although libc.a appears to export it, the GNU ld linker complains that
//! it can't resolve the symbol so here's a local version that cures the
//! problem.
//!
//! \return Does not return.
//
//*****************************************************************************
void abort(void) {
	while (1) {
	}
}

//*****************************************************************************
//
//! This is a lightweight substitute for the C runtime time() function.
//!
//! \param pTime is a pointer to storage for the returned time. If NULL, the
//! parameter is ignored.
//!
//! \note MatrixSSL uses time() purely to get the first 4 values of the
//! server random block. We don't have access to the real UTC time so just
//! return the second count since the system started instead. If your system
//! has a UTC reference, this function could be replaced with one that returns
//! the actual time.
//!
//! \return Returns the number of seconds since the system started the SSL
//! timer. The same value is also returned in *pTime if pTime is not NULL.
//
//*****************************************************************************
time_t NoOSTime(time_t *pTime) {
	unsigned long ulSeconds = g_ulSeconds;

	if (pTime) {
		*pTime = ulSeconds;
	}

	return (ulSeconds);
}

//*****************************************************************************
//
// Wrappers for C runtime memory management functions. We use the public domain
// bget library as our heap manager and wrap the standard C functions around
// its APIs.
//
//*****************************************************************************

//*****************************************************************************
//
//! Overrides the default timer selection used to provide system time to the
//! SSL library.
//!
//! \param ulTimerBase is the base address of the hardware timer that the OS
//! layer should use to provide a system tick to the SSL library.
//!
//! This function allows a client to chose which of the available hardware
//! timers should be used to provide the time reference for the SSL library.
//! By default, timer 0 is used.
//!
//! \note This call is only valid prior to sslOpenOsdep(). If called after this,
//! a debug build will throw an assert failure. The caller is responsible for
//! having installed the appropriate timer interrupt handler vector prior
//! to making this call.
//!
//! \Returns None.
//
//*****************************************************************************
void sslSetHardwareTimer(unsigned long ulTimerBase) {

}

//*****************************************************************************
//
//! Initializes the OS support layer.
//!
//! \param None.
//!
//! This function is called during processing of martixSslOpen to initialize
//! the OS support on behalf of the SSL library. It sets up the heap used to
//! provide buffers to the module and initializes the hardware timer used to
//! generate a time reference.
//!
//! \note It is assumed that the caller will have enabled the relevant
//! timer using a call to SysCtlPeripheralEnable prior to making this
//! call. If this is not done, this function will generate a bus fault.
//!
//! \return Returns 0 on success or a negative number on failure.
//
//*****************************************************************************
int32 sslOpenOsdep(void) {

//    g_bSslTimerInitialized = true;

	//
	// Seed the "random" number generator with something that is board specific
	// like, for example, the MAC address. This is typically stored in the
	// flash user registers.

	SeedRandom(5567);

	//
	// Tell the caller all is well.
	//
	return (0);
}

int32 sslCloseOsdep(void) {
	//
	// Stop our timer and unregister its interrupt.
	//

	//   g_bSslTimerInitialized = false;

	//
	// Let the caller know everything went as planned.
	//
	return 0;
}

//*****************************************************************************
//
//! Fills the supplied buffer with random bytes.
//!
//! \param bytes is a pointer to the buffer into which random data is to be
//! written.
//! \param size is the number of bytes of random data to write to the supplied
//! buffer.
//!
//! This function generates random data for use by the SSL library. The
//! algorithm used here to generate the random data is a rather simple shift/
//! XOR process seeded with a number derived from the system's ethernet MAC
//! address. It should be pretty random but whether it is "cryptographically
//! random" or not is debatable. Unfortunately, I don't have any alpha particle
//! sources or cosmic ray counters around so this will have to do for now.
//!
//! \return Returns -1 on failure or the number of bytes of random data
//! written to buffer "bytes" if successful.
//
//*****************************************************************************
int32 sslGetEntropy(unsigned char *bytes, uint32 size) {
	int32 iLoop;
	unsigned long ulRand;

	for (iLoop = 0; iLoop < size; iLoop++) {
		//
		// Get 4 bytes of random data.
		//
		ulRand = GetRandom();

		//
		// Save one byte of the random unsigned long value into the output
		// buffer.
		//
		bytes[iLoop] = (unsigned char) (ulRand & 0xFF);
	}

	//
	// Tell the caller how many bytes of "random" data we generated.
	//
	return (size);
}



void psBreak(void) {
	// ASSERT(0);
}

int32 psGetTime(psTime_t *t) {
	NoOSTime((time_t *) &(t->sec));
	return 0;
}

/*
 Return the delta in milliseconds between two time values
 */
int32 psDiffMsecs(psTime_t then, psTime_t now) {
	long lSeconds;
	long lExtraMilliseconds;

	//
	// Consider first the easiest case when the two times are within the
	// same second. This, of course, assimes that then and now are correctly
	// ordered with now later than then.
	//
	if (now.sec == then.sec) {
		return ((now.usec - then.usec) / 1000);
	} else {
		//
		// The times are in different seconds so consider the seconds and
		// microseconds separately.
		//
		lSeconds = (now.sec - then.sec) - 1;
		lExtraMilliseconds = ((1000000 - then.usec) + now.usec) / 1000;

		//
		// Guard against overflow. If the time difference is too large to
		// hold in a long, send back the largest value allowable rather than
		// wrapping and sending something completely bogus.
		//
		if (lSeconds > (MAX_OVERFLOW_FREE_SECONDS - lExtraMilliseconds)) {
			//
			// Calculation would overflow so return the largest different we
			// can represent in a single (signed) long.
			//
			return (0x7FFFFFFF);
		} else {
			//
			// No overflow worries so calculate the number of milliseconds
			// by multiplying seconds by 1000 and adding the extra bit.
			//
			return ((lSeconds * 1000) + lExtraMilliseconds);
		}
	}
}

int32 psCompareTime(psTime_t a, psTime_t b) {
	if ((a.sec > b.sec) || ((a.sec == b.sec) && (a.usec > b.usec))) {
		return (0);
	} else {
		return (1);
	}
}

int32 psCoreOpen(void) {
	return 0;
}
void psCoreClose(void) {
}

void _psTrace(char *msg)
{
	ssl_printf(msg);
}

/* message should contain one %s, unless value is NULL */
void _psTraceStr(char *message, char *value)
{
    if (value) {
    	ssl_printf(message, value);
    } else {
    	ssl_printf(message);
    }
}

/* message should contain one %d */
void _psTraceInt(char *message, int32 value)
{
	ssl_printf(message, value);
}

/* message should contain one %p */
void _psTracePtr( char *message, void *value)
{
	ssl_printf(message, value);
}

