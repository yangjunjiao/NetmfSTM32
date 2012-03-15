////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"

#define RTC_TICKS_PER_SEC      1000ul
#define CLR_TICKS_PER_SEC  10000000ul
#define RTC_TICK_TO_CLR_TICK (CLR_TICKS_PER_SEC/RTC_TICKS_PER_SEC)

extern void UARTSend(const char *pucBuffer);
//--//

volatile UINT64 SysTimeTickCnt = 0;
volatile UINT64 NextCompareTick = 0x0000FFFFFFFFFFFFull;
//volatile UINT64 CompareTick = 0;

//*****************************************************************************
//
//  Function Prototype
//
//*****************************************************************************
UINT64 HAL_Time_MicrosecondsToTicks( INT64 ms )
{
    return ((ms * RTC_TICKS_PER_SEC)/1000);
}

//*****************************************************************************
//
// SysTimeISR
//
//*****************************************************************************
void SysTimeISR(void)
{
    static int Cnt1Sec = 0;
    static unsigned char LedState;
    
    TimerIntClear(TIMER2_BASE, TIMER_TIMB_TIMEOUT);

    SysTimeTickCnt++;
    Cnt1Sec++;
    
    if( Cnt1Sec >= RTC_TICKS_PER_SEC)
    {
        Cnt1Sec = 0;
        GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, (LedState^=GPIO_PIN_3));
    }
    
    if( SysTimeTickCnt >= NextCompareTick)
    {
        HAL_COMPLETION::DequeueAndExec();
    }
}


BOOL HAL_Time_Initialize()
{
    SysTimeTickCnt = 0;
    NextCompareTick = 0x0000FFFFFFFFFFFFull;
    
    SysCtlPeripheralClockGating(true);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0);
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralDeepSleepEnable(SYSCTL_PERIPH_TIMER2);
    
    TimerConfigure(  TIMER2_BASE,TIMER_CFG_16_BIT_PAIR | TIMER_CFG_A_ONE_SHOT | TIMER_CFG_B_PERIODIC );
    TimerPrescaleSet(TIMER2_BASE,TIMER_B,32);
    TimerLoadSet(    TIMER2_BASE,TIMER_B, SysCtlClockGet()/32/RTC_TICKS_PER_SEC);
    TimerIntEnable(  TIMER2_BASE,TIMER_TIMB_TIMEOUT);
    TimerDisable(    TIMER2_BASE, TIMER_A);
    TimerEnable(     TIMER2_BASE, TIMER_B);
    TimerIntRegister(TIMER2_BASE,TIMER_B,SysTimeISR);
    IntEnable(       INT_TIMER2B);
    return TRUE;
}

BOOL HAL_Time_Uninitialize()
{
    TimerIntDisable(TIMER2_BASE,TIMER_TIMB_TIMEOUT);
    TimerDisable(   TIMER2_BASE, TIMER_B);
    IntUnregister(  INT_TIMER2B);
    IntDisable(     INT_TIMER2B);
    return TRUE;
}

UINT64 HAL_Time_CurrentTicks()
{
    return SysTimeTickCnt;
}

INT64 HAL_Time_TicksToTime( UINT64 Ticks )
{
    return (Ticks * RTC_TICK_TO_CLR_TICK );
}

INT64 HAL_Time_CurrentTime()
{
    return ( ((INT64)SysTimeTickCnt) * RTC_TICK_TO_CLR_TICK);
}

void HAL_Time_SetCompare( UINT64 CompareValue )
{
    NextCompareTick = (CompareValue/RTC_TICK_TO_CLR_TICK);
}

void HAL_Time_Sleep_MicroSeconds( UINT32 uSec )
{
    //SysCtlPeripheralClockGating(true);
    SysCtlPeripheralDeepSleepEnable(SYSCTL_PERIPH_TIMER2);
    TimerDisable(    TIMER2_BASE, TIMER_B);
    TimerLoadSet(    TIMER2_BASE, TIMER_B, ((UINT64)SysCtlClockGet()/32 * uSec) /1000000ul);
    TimerEnable(     TIMER2_BASE, TIMER_B);
    SysCtlDeepSleep();
    //SysCtlPeripheralDeepSleepDisable(SYSCTL_PERIPH_TIMER2);
    TimerIntDisable(  TIMER2_BASE,TIMER_TIMB_TIMEOUT);
    SysTimeTickCnt += HAL_Time_MicrosecondsToTicks(uSec);
    TimerLoadSet(    TIMER2_BASE,TIMER_B, SysCtlClockGet()/32/RTC_TICKS_PER_SEC);
    TimerIntEnable(  TIMER2_BASE,TIMER_TIMB_TIMEOUT);
}

void HAL_Time_Sleep_MicroSeconds_InterruptEnabled( UINT32 uSec )
{
    HAL_Time_Sleep_MicroSeconds(uSec);
}

void HAL_Time_GetDriftParameters  ( INT32* a, INT32* b, INT64* c )
{
    *a = 1;
    *b = 1;
    *c = 0;
}

UINT32 CPU_SystemClock()
{
    return (SysCtlClockGet());
}


UINT32 CPU_TicksPerSecond()
{
    return RTC_TICKS_PER_SEC;
}

UINT64 CPU_MillisecondsToTicks( UINT64 Ticks )
{
    return ((Ticks * RTC_TICKS_PER_SEC)/1000);
}

UINT64 CPU_MillisecondsToTicks( UINT32 Ticks32 )
{
    return ((Ticks32 * RTC_TICKS_PER_SEC)/1000);
}

UINT64 CPU_MicrosecondsToTicks( UINT64 uSec )
{
    return ((uSec * RTC_TICKS_PER_SEC)/1000000ul);
}

UINT32 CPU_MicrosecondsToTicks( UINT32 uSec )
{
    return ((uSec * RTC_TICKS_PER_SEC)/1000000ul);
}

UINT32 CPU_MicrosecondsToSystemClocks( UINT32 uSec )
{
    return ( (SysCtlClockGet()/1000000ul) * uSec );
}

UINT64 CPU_TicksToTime( UINT64 Ticks )
{
    return (Ticks * RTC_TICK_TO_CLR_TICK );
}

UINT64 CPU_TicksToTime( UINT32 Ticks32 )
{
    return (Ticks32 * RTC_TICK_TO_CLR_TICK );
}

