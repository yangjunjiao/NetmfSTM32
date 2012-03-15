////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __LM3S__USART_H__
#define __LM3S__USART_H__   1
#include <tinyhal.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_uart.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"


typedef struct STRUCT_LM3S_USART
{
    unsigned long ulBaseAddr;
    unsigned long ulIOBaseAddr;
    unsigned long ulPeripheralNo;
    unsigned long ulPeripheralGpioNo;
    unsigned long ulIntNo;
    unsigned long ulIntFlags;
    unsigned long ulTxFifoLevel;
    unsigned long ulRxFifoLevel;
    BOOL          bFifoEnable;
    unsigned char ucGpioPins;
    unsigned long ulConfig;
    int BaudRate;
    int Parity;
    int DataBits;
    int StopBits;
    int FlowValue;
    
}LM3S_USART;


//////////////////////////////////////////////////////////////////////////////
// LM3S_USART_Driver
//
struct LM3S_USART_Driver
{
    static const UINT32 c_RefFlagRx = 0x01;
    static const UINT32 c_RefFlagTx = 0x02;

    //--//
    INT8 m_RefFlags[TOTAL_USART_PORT];

    //--//
    static BOOL Initialize( int comPort, int baudRate, int Parity, int DataBits, int StopBits, int flowValue );
    static BOOL Uninitialize( int comPort );
    static BOOL TxBufferEmpty( int comPort );
    static BOOL TxShiftRegisterEmpty( int comPort );
    static void WriteCharToTxBuffer( int comPort,  UINT8 c );
    static void TxBufferEmptyInterruptEnable( int comPort, BOOL enable );
    static BOOL TxBufferEmptyInterruptState( int comPort );
    static void RxBufferFullInterruptEnable( int comPort, BOOL enable );
    static BOOL RxBufferFullInterruptState( int comPort );
    static BOOL TxHandshakeEnabledState( int comPort );
    static void ProtectPins( int comPort, BOOL on );
    static void USART_ISR( void* Param );
    static void GetPins( int comPort,  GPIO_PIN& rxPin, GPIO_PIN& txPin,GPIO_PIN& ctsPin, GPIO_PIN& rtsPin );    
    static void BaudrateBoundary( int ComPortNum, UINT32& maxBaudrateHz, UINT32& minBaudrateHz );
    static BOOL IsBaudrateSupported( int ComPortNum, UINT32 & BaudrateHz );    

private:
    static void RefFlags( int ComPortNum, INT8 RefFlags, BOOL Add );
};
//
// LM3S_USART_Driver
//////////////////////////////////////////////////////////////////////////////

#endif //_LM3S__USART_H_1
