////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 
// This file is part of the Microsoft .NET Micro Framework Porting Kit Code Samples and is unsupported. 
// Copyright (C) Microsoft Corporation. All rights reserved. Use of this sample source code is subject to 
// the terms of the Microsoft license agreement under which you licensed this sample source code. 
// 
// THIS SAMPLE CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
// 
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

unsigned long UART0_Int_Flags = UART_INT_RX | UART_INT_RT;

void CPU_USART_TxBufferEmptyInterruptEnable( int ComPortNum, BOOL Enable );
//--//
//*****************************************************************************
//
// CPU_USART_ISR interrupt.
//
//*****************************************************************************
void CPU_USART_ISR(void)
{
    unsigned long ulR0;
    unsigned long RxData;
    char c;
    UINT32 comPort = 0;
    
    // Get the interrrupt status.
    ulR0 = UARTIntStatus(UART0_BASE,true);
    
    // Clear the asserted interrupts.
    UARTIntClear(UART0_BASE, ulR0);

    if(ulR0 & (UART_INT_RX | UART_INT_RT) )
    {
        while(UARTCharsAvail(UART0_BASE))
        {
            RxData = UARTCharGetNonBlocking(UART0_BASE);
            USART_AddCharToRxBuffer( comPort, (char) RxData );
            Events_Set( SYSTEM_EVENT_FLAG_COM_IN );
        }
    }
    else if(ulR0 & UART_INT_TX)
    {
        
        while(UARTSpaceAvail(UART0_BASE))
        {
            if( USART_RemoveCharFromTxBuffer( comPort, c ) ) 
            {
                UARTCharPut(UART0_BASE,c);
            }
            else
            {
                // disable further Tx interrupts since we are level triggered
                CPU_USART_TxBufferEmptyInterruptEnable( comPort, FALSE );
                break;
            }
        }
        /*
        if( USART_RemoveCharFromTxBuffer( comPort, c ) ) 
        {
            UARTCharPut(UART0_BASE,c);
        }
        else
        {
            // disable further Tx interrupts since we are level triggered
            CPU_USART_TxBufferEmptyInterruptEnable( comPort, FALSE );
        }
        */
        Events_Set( SYSTEM_EVENT_FLAG_COM_OUT );
    }
}

BOOL CPU_USART_Initialize( int ComPortNum, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue )
{
    if( ComPortNum != 0) return TRUE;
    
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralSleepEnable(SYSCTL_PERIPH_GPIOA);
    // Set GPIO A0 and A1 as UART pins.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BaudRate,
                        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
                         UART_CONFIG_PAR_NONE));
    
    //UARTFIFODisable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX6_8);
    UARTIntEnable(UART0_BASE, UART0_Int_Flags);
    UARTIntRegister(UART0_BASE, CPU_USART_ISR);
    IntEnable(INT_UART0);
    return TRUE;
}

BOOL CPU_USART_Uninitialize( int ComPortNum )
{
    if( ComPortNum != 0) return TRUE;
    
    IntDisable(INT_UART0);
    return TRUE;
}

BOOL CPU_USART_TxBufferEmpty( int ComPortNum )
{
    if( ComPortNum != 0) return TRUE;
    
    return UARTSpaceAvail(UART0_BASE);
}

BOOL CPU_USART_TxShiftRegisterEmpty( int ComPortNum )
{
    if( ComPortNum != 0) return TRUE;
    
    if(UARTBusy(UART0_BASE) == true)
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

void CPU_USART_WriteCharToTxBuffer( int ComPortNum, UINT8 c )
{
    if( ComPortNum != 0) return;
    
    UARTCharPut(UART0_BASE,c);
    
    if((UART0_Int_Flags & UART_INT_TX) == 0)
    {
        UART0_Int_Flags |= UART_INT_TX;
	      UARTIntEnable(UART0_BASE, UART0_Int_Flags);
    }
}

void CPU_USART_TxBufferEmptyInterruptEnable( int ComPortNum, BOOL Enable )
{
    if( ComPortNum != 0) return;
    
    if(Enable)
    {
        if(1)//((UART0_Int_Flags & UART_INT_TX) == 0)
        {
            char c;
            
            UARTIntDisable(UART0_BASE, UART_INT_TX);
            
            while(UARTSpaceAvail(UART0_BASE))
            {
                if( USART_RemoveCharFromTxBuffer( 0, c ) ) 
                {
                    UARTCharPut(UART0_BASE,c);
                }
                else
                {
                    break;
                }
            }
            UART0_Int_Flags |= UART_INT_TX;
	          UARTIntEnable(UART0_BASE, UART0_Int_Flags);
        }
    }
    else
    {
        UART0_Int_Flags &= ~UART_INT_TX;
	      UARTIntDisable(UART0_BASE, UART_INT_TX);
    }
}

BOOL CPU_USART_TxBufferEmptyInterruptState( int ComPortNum )
{
    unsigned long ulStatus;
    
    if( ComPortNum != 0) return FALSE;
    
    ulStatus = UARTIntStatus(UART0_BASE,true);
    
    return ((ulStatus & UART_INT_TX) ? TRUE:FALSE);
}

void CPU_USART_RxBufferFullInterruptEnable( int ComPortNum, BOOL Enable )
{
    if( ComPortNum != 0) return;
    
    UART0_Int_Flags |= UART_INT_RX;
    UARTIntEnable(UART0_BASE, UART0_Int_Flags);
}

BOOL CPU_USART_RxBufferFullInterruptState( int ComPortNum )
{
    unsigned long ulStatus;
    
    if( ComPortNum != 0) return FALSE;
    
    ulStatus = UARTIntStatus(UART0_BASE,true);
    
    return ((ulStatus & UART_INT_RX) ? TRUE:FALSE);
}

void CPU_USART_ProtectPins( int ComPortNum, BOOL On )
{
}

UINT32 CPU_USART_PortsCount()
{
    return 1;
}

void CPU_USART_GetPins( int ComPortNum, GPIO_PIN& rxPin, GPIO_PIN& txPin,GPIO_PIN& ctsPin, GPIO_PIN& rtsPin )
{   
    rxPin = GPIO_PIN_NONE; 
    txPin = GPIO_PIN_NONE; 
    ctsPin= GPIO_PIN_NONE; 
    rtsPin= GPIO_PIN_NONE; 

    return;
}

BOOL CPU_USART_SupportNonStandardBaudRate ( int ComPortNum )
{
    return FALSE;
}

void CPU_USART_GetBaudrateBoundary( int ComPortNum, UINT32& maxBaudrateHz, UINT32& minBaudrateHz )
{
    maxBaudrateHz = SysCtlClockGet()/16;
    minBaudrateHz = 1200;
}

BOOL CPU_USART_IsBaudrateSupported( int ComPortNum, UINT32 & BaudrateHz )
{   
    if( (BaudrateHz <= (SysCtlClockGet()/16)) && (BaudrateHz >= 1200) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL CPU_USART_TxHandshakeEnabledState( int comPort )
{
    return FALSE;
}




