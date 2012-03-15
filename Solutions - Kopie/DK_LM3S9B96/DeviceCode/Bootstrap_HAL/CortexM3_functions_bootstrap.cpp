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
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "set_pinout.h"
#include "../BlockStorage_HAL/ssiflash.h"

//#define CODE_IN_EXT_MEM   1

extern void PrepareImageRegions();

//*****************************************************************************
//
// Send a string to the UART.
//
//*****************************************************************************
void UARTSend(unsigned long ulUartAddr, const char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        //
        // Write the next character to the UART.
        //
        UARTCharPut(ulUartAddr, *pucBuffer++);
    }
}

//--//

void BootstrapCode()
{
    const char CompanyStr[]    = "Golden IC Technology Co.,Ltd.\n";
    const char CompanyWebStr[] = "Web: www.Golden-IC.com\n";
    
#ifdef CODE_IN_EXT_MEM
    PrepareImageRegions();
    g_eDaughterType = DAUGHTER_NONE;//DAUGHTER_SRAM_FLASH;
#else
    // Set the system clock to run at 80MHz from the PLL
    SysCtlClockSet(SYSCTL_SYSDIV_3 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // Set the device pinout appropriately for this board.  This also enables
    // all the GPIO ports required.
    PinoutSet();
    PrepareImageRegions();
#endif
    
    //Enable UART0 ,Configure the UART for 115,200, 8-N-1 operation.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTSend(UART0_BASE,CompanyStr);
    UARTSend(UART0_BASE,CompanyWebStr);
    
    //Enable UART1 ,Configure the UART for 115,200, 8-N-1 operation.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTSend(UART1_BASE,CompanyStr);
    UARTSend(UART1_BASE,CompanyWebStr);
    
    //
}
