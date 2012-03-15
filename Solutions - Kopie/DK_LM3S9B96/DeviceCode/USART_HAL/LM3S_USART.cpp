////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "LM3S_USART.h"

//#define UART_LOOP_BACK

//--//
LM3S_USART LM3S_UART_SETTING[3]=
{
    //UART0
    {
        UART0_BASE,               //unsigned long ulBaseAddr;
        GPIO_PORTA_BASE,          //unsigned long ulIOBaseAddr;
        SYSCTL_PERIPH_UART0,      //unsigned long ulPeripheralNo;
        SYSCTL_PERIPH_GPIOA,      //unsigned long ulPeripheralGpioNo;
        INT_UART0,                //unsigned long ulIntNo;
        0x00000000,               //unsigned long ulIntFlags;
        UART_FIFO_TX1_8,          //unsigned long ulTxFifoLevel;
        UART_FIFO_RX6_8,          //unsigned long ulRxFifoLevel;
        TRUE,                     //BOOL bFifoEnable;
        (GPIO_PIN_0 | GPIO_PIN_1) //unsigned char ucGpioPins;
    },
    
    //UART1
    {
        UART1_BASE,               //unsigned long ulBaseAddr;
        GPIO_PORTB_BASE,          //unsigned long ulIOBaseAddr;
        SYSCTL_PERIPH_UART1,      //unsigned long ulPeripheralNo;
        SYSCTL_PERIPH_GPIOB,      //unsigned long ulPeripheralGpioNo;
        INT_UART1,                //unsigned long ulIntNo;
        0x00000000,               //unsigned long ulIntFlags;
        UART_FIFO_TX1_8,          //unsigned long ulTxFifoLevel;
        UART_FIFO_RX6_8,          //unsigned long ulRxFifoLevel;
        TRUE,                     //BOOL bFifoEnable;
        (GPIO_PIN_0 | GPIO_PIN_1) //unsigned char ucGpioPins;
    },

    //UART2
    {
        UART2_BASE,               //unsigned long ulBaseAddr;
        GPIO_PORTD_BASE,          //unsigned long ulIOBaseAddr;
        SYSCTL_PERIPH_UART2,      //unsigned long ulPeripheralNo;
        SYSCTL_PERIPH_GPIOD,      //unsigned long ulPeripheralGpioNo;
        INT_UART2,                //unsigned long ulIntNo;
        0x00000000,               //unsigned long ulIntFlags;
        UART_FIFO_TX1_8,          //unsigned long ulTxFifoLevel;
        UART_FIFO_RX6_8,          //unsigned long ulRxFifoLevel;
        TRUE,                     //BOOL bFifoEnable;
        (GPIO_PIN_5 | GPIO_PIN_6) //unsigned char ucGpioPins;
    }
};


LM3S_USART_Driver g_LM3S_USART_Driver;

void LM3S_UART0_ISR( void* param )
{
    LM3S_USART_Driver::USART_ISR((void*)0);
}
void LM3S_UART1_ISR( void* param )
{
    LM3S_USART_Driver::USART_ISR((void*)1);
}
void LM3S_UART2_ISR( void* param )
{
    LM3S_USART_Driver::USART_ISR((void*)2);
}

HAL_CALLBACK_FPN LM3S_USART_ISR_TAB[3]=
{
   LM3S_UART0_ISR,
   LM3S_UART1_ISR,
   LM3S_UART2_ISR,
};

BOOL LM3S_USART_Driver::Initialize( int comPort, int BaudRate, int Parity, int DataBits, int StopBits, int FlowValue )
{
    BOOL fRet = TRUE;
    unsigned long ulConfig = 0;
    
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    GLOBAL_LOCK(irq);
    // Disable interrupts
    IntDisable(usart.ulIntNo);

    //Enable UART and related GPIO
    SysCtlPeripheralEnable(usart.ulPeripheralNo);
    SysCtlPeripheralEnable(usart.ulPeripheralGpioNo);
    
    usart.BaudRate  = BaudRate;
    usart.DataBits  = DataBits;
    usart.Parity    = Parity;
    usart.StopBits  = StopBits;
    usart.FlowValue = FlowValue;
    
    //Convert Paramaters to Config bit flags
    switch(Parity)
    {
        case USART_PARITY_ODD:
            ulConfig |= UART_CONFIG_PAR_ODD;
            break;
        case USART_PARITY_EVEN:
            ulConfig |= UART_CONFIG_PAR_EVEN;
            break;
        case USART_PARITY_MARK:
            ulConfig |= UART_CONFIG_PAR_ONE;
            break;
        case USART_PARITY_SPACE:
            ulConfig |= UART_CONFIG_PAR_ZERO;
            break;
        default: // not supported
            fRet = FALSE;
            //fall through for default
        case USART_PARITY_NONE:
            ulConfig |= UART_CONFIG_PAR_NONE;
            break;
    }
    
    switch(DataBits)
    {
        case 5:
            ulConfig |= UART_CONFIG_WLEN_5;
            break;
        case 6:
            ulConfig |= UART_CONFIG_WLEN_6;
            break;
        case 7:
            ulConfig |= UART_CONFIG_WLEN_7;
            break;
        default: // not supported
            fRet = FALSE;
            //fall through for default
        case 8:
            ulConfig |= UART_CONFIG_WLEN_8;
            break;
    }
    
    switch(StopBits)
    {
        case USART_STOP_BITS_ONE:
            // this board doesn't appear to work with 1 stop bits set
            ulConfig |= UART_CONFIG_STOP_ONE;
            break;
        case USART_STOP_BITS_TWO:
            ulConfig |= UART_CONFIG_STOP_TWO;
            break;
            
        // not supported
        case USART_STOP_BITS_ONEPOINTFIVE:
        case USART_STOP_BITS_NONE:
        default:
            fRet = FALSE;
            break;
    }
    
    usart.ulConfig = ulConfig;
    
    // Configure the UART for 115,200, 8-N-1 operation.
    UARTConfigSetExpClk(usart.ulBaseAddr, SysCtlClockGet(), usart.BaudRate, usart.ulConfig);
    if(usart.bFifoEnable)
    {
        UARTFIFOEnable(usart.ulBaseAddr);
        UARTFIFOLevelSet(usart.ulBaseAddr,UART_FIFO_TX1_8,UART_FIFO_RX6_8);
    }
    else
    {
        UARTFIFODisable(usart.ulBaseAddr);
    }
    
    // inits GPIO pins
    ProtectPins( comPort, FALSE );
    RefFlags( comPort, c_RefFlagRx, TRUE );
    
    if(!CPU_INTC_ActivateInterrupt( usart.ulIntNo, LM3S_USART_ISR_TAB[comPort], (void*)(size_t)comPort) )
    {
            return FALSE;
    }
    
    return fRet;
}


BOOL LM3S_USART_Driver::Uninitialize( int comPort )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    GLOBAL_LOCK(irq);

    // uninits GPIO pins
    ProtectPins( comPort, TRUE );
    CPU_INTC_DeactivateInterrupt(usart.ulIntNo);
    //UARTDisable(usart.ulPeripheralNo);
    //SysCtlPeripheralDisable(usart.ulPeripheralNo);
    return TRUE;
}

void LM3S_USART_Driver::RefFlags( int comPort, INT8 RefFlags, BOOL Add )
{
    ASSERT_IRQ_MUST_BE_OFF();

    if(Add)
    {
        // time to turn on the port?
        if(0 == g_LM3S_USART_Driver.m_RefFlags[comPort])
        {
            ASSERT(RefFlags);
            // enable clock to peripheral
            // TODO TODO TODO: provide an API to allow power to peripheral
            // CPU_CMU_PeripheralClock((c_COM1 == comPort) ? MM9637A_CMU::MCLK_EN__USART0 : MM9637A_CMU::MCLK_EN__USART1, TRUE);
        }
        
        g_LM3S_USART_Driver.m_RefFlags[comPort] |= RefFlags;
    }
    else
    {
        g_LM3S_USART_Driver.m_RefFlags[comPort] &= ~RefFlags;
        
        // time to turn off this com port?
        if(0 == g_LM3S_USART_Driver.m_RefFlags[comPort])
        {
            // turn off the peripheral clock
            // we should wait max 200uSec (2 char shift time, 115200) for this so we don't lose
            // characters in the shift register.  Ollie has no int for this last one, bummer.
            // we usually expect TxBufferEmpty most times, so really it is about 100uSec @ 115200
            // don't clear RefFlags before these waits, otherwise they return immediately
            while(!TxBufferEmpty       ( comPort ));
            while(!TxShiftRegisterEmpty( comPort ));
            
            // enable clock to peripheral
            // TODO TODO TODO: provide an API to shut down power to peripheral
             //CPU_CMU_PeripheralClock( (c_COM1 == comPort) ? MM9637A_CMU::MCLK_EN__USART0 : MM9637A_CMU::MCLK_EN__USART1, FALSE );
        }
    }
}

BOOL LM3S_USART_Driver::TxBufferEmpty( int comPort )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];

    // if the port TX is disabled, we dump chars to nowhere, pretend we have room
    if(0 != (c_RefFlagTx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        return((HWREG(usart.ulBaseAddr + UART_O_FR) & UART_FR_TXFF) ? FALSE : TRUE);
    }
    
    return TRUE;
}

BOOL LM3S_USART_Driver::TxShiftRegisterEmpty( int comPort )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    // if the port is disabled, we dump chars to nowhere
    if(0 != (c_RefFlagTx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        return((HWREG(usart.ulBaseAddr + UART_O_FR) & UART_FR_BUSY) ? FALSE : TRUE);
    }
    
    return TRUE;
}

void LM3S_USART_Driver::WriteCharToTxBuffer( int comPort, UINT8 c )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    HWREG(usart.ulBaseAddr + UART_O_DR) = c;
}

void LM3S_USART_Driver::TxBufferEmptyInterruptEnable( int comPort, BOOL enable )
{
    
    UINT32 previousRefFlags = g_LM3S_USART_Driver.m_RefFlags[comPort];
    BOOL   previousState;

    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    GLOBAL_LOCK(irq);
    
    if(0 != (c_RefFlagTx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        previousState = (0 != (usart.ulIntFlags & UART_INT_TX) );     
    }
    else
    {
        previousState = FALSE;
    }

    if(enable)
    {
        // only do this work if the Tx INT is off
        if(!previousState)
        {
            // TX on doesn't necessarily add a refcount
            RefFlags( comPort, c_RefFlagTx, TRUE );

            usart.ulIntFlags |= UART_INT_TX;
            UARTIntEnable(usart.ulBaseAddr, UART_INT_TX);
        }
    }
    else
    {
        // if it is on, then turn it off
        if(previousState)
        {
            usart.ulIntFlags &= ~UART_INT_TX;
            UARTIntDisable(usart.ulBaseAddr, UART_INT_TX );
            RefFlags( comPort, c_RefFlagTx, FALSE );
        }
    }

    // tell the driver when we toggle in our out of powersave mode
    if(0 == g_LM3S_USART_Driver.m_RefFlags[comPort])
    {
        if(0 != previousRefFlags)
        {
            USART_PowerSave( comPort, 1 );
        }
    }
    else
    {
        if(0 == previousRefFlags)
        {
            USART_PowerSave( comPort, 0 );
        }
    }
}

BOOL LM3S_USART_Driver::TxBufferEmptyInterruptState( int comPort )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];

    if(0 != (c_RefFlagTx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        return (0 != (usart.ulIntFlags & UART_INT_TX) );
    }

    return FALSE;
}

void LM3S_USART_Driver::RxBufferFullInterruptEnable( int comPort, BOOL Enable )
{
    
    UINT32 previousRefFlags = g_LM3S_USART_Driver.m_RefFlags[comPort];
    BOOL   previousState;

    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    GLOBAL_LOCK(irq);

    if(0 != (c_RefFlagRx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        previousState = (0 != (usart.ulIntFlags & UART_INT_RX) );
    }
    else
    {
        previousState = FALSE;
    }

    if(Enable)
    {
        // only do this work if the RX INT is off
        if(!previousState)
        {
            RefFlags( comPort, c_RefFlagRx, TRUE );

            usart.ulIntFlags |= (UART_INT_RX|UART_INT_RT);
            UARTIntEnable(usart.ulBaseAddr, (UART_INT_RX|UART_INT_RT) );
        }
    }
    else
    {
        // if it is on, then turn it off
        if(previousState)
        {
            usart.ulIntFlags &= ~(UART_INT_RX|UART_INT_RT);
            UARTIntDisable(usart.ulBaseAddr, (UART_INT_RX|UART_INT_RT) );
            RefFlags( comPort, c_RefFlagRx, FALSE );
        }
    }

    // tell the driver when we toggle in our out of powersave mode
    if(0 == g_LM3S_USART_Driver.m_RefFlags[comPort])
    {
        if(0 != previousRefFlags)
        {
            USART_PowerSave( comPort, 1 );
        }
    }
    else
    {
        if(0 == previousRefFlags)
        {
            USART_PowerSave( comPort, 0 );
        }
    }
}

BOOL LM3S_USART_Driver::RxBufferFullInterruptState( int comPort )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
     if(0 != (c_RefFlagRx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
    {
        return (0 != (usart.ulIntFlags & UART_INT_RX) );
    }

    return FALSE;
}

void LM3S_USART_Driver::USART_ISR( void* param )
{
    UINT32     comPort = (UINT32)param;
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    char c;
    unsigned long Status;
    
    // we lock since we are low priority and anyone might try to add a char in another ISR
    GLOBAL_LOCK(irq);
     
    // if the charger came in and turned off the serial port, bail now, as the source
    // will be going away
    
    // Get the interrrupt status.
    Status = HWREG(usart.ulBaseAddr + UART_O_MIS);
    // Clear the asserted interrupts.
    HWREG(usart.ulBaseAddr + UART_O_ICR) = Status;
    
    if(Status & (UART_INT_RX | UART_INT_RT) )
    {
        if( !(HWREG(usart.ulBaseAddr + UART_O_FR) & UART_FR_RXFE)  ) //if Rx FIFO is not empty
        {
            do
            {
                c = HWREG(usart.ulBaseAddr + UART_O_DR);
                #ifdef UART_LOOP_BACK
                HWREG(usart.ulBaseAddr + UART_O_DR) = (UINT32)c;
                #endif
                USART_AddCharToRxBuffer( comPort, c );
            }
            while(!(HWREG(usart.ulBaseAddr + UART_O_FR) & UART_FR_RXFE)  ); //while Rx FIFO is not empty
                
            Events_Set( SYSTEM_EVENT_FLAG_COM_IN );
        }
    }
    
    if(Status & UART_INT_TX)
    {
        if(0 == (c_RefFlagTx & g_LM3S_USART_Driver.m_RefFlags[comPort]))
        {
            return;
        }
        
        if(USART_RemoveCharFromTxBuffer( comPort, c ))
        {
            HWREG(usart.ulBaseAddr + UART_O_DR) = c;
        }
        else
        {
            // disable further Tx interrupts since we are level triggered
            TxBufferEmptyInterruptEnable( comPort, FALSE );
        }
        Events_Set( SYSTEM_EVENT_FLAG_COM_OUT );
    }
}

BOOL LM3S_USART_Driver::TxHandshakeEnabledState( int comPort )
{
    // TODO: Add handshake code
    return TRUE;
}

void LM3S_USART_Driver::ProtectPins( int comPort, BOOL on )
{
    LM3S_USART &usart = LM3S_UART_SETTING[comPort];
    
    GLOBAL_LOCK(irq);
    
    
    if(on)
    {
        // Config the GPIO as Input to save power
        GPIOPinTypeGPIOInput(usart.ulIOBaseAddr, usart.ucGpioPins);
        
        RxBufferFullInterruptEnable( comPort, FALSE );
        TxBufferEmptyInterruptEnable( comPort, FALSE );
    }
    else
    {
        // Set GPIO pins as UART pins.
        GPIOPinTypeUART(usart.ulIOBaseAddr, usart.ucGpioPins);
        TxBufferEmptyInterruptEnable( comPort, TRUE );
        RxBufferFullInterruptEnable( comPort, TRUE );
    }
}

void LM3S_USART_Driver::GetPins(int comPort, GPIO_PIN& rxPin, GPIO_PIN& txPin, GPIO_PIN& ctsPin, GPIO_PIN& rtsPin)
{
    rxPin = GPIO_PIN_NONE; 
    txPin = GPIO_PIN_NONE; 
    ctsPin= GPIO_PIN_NONE; 
    rtsPin= GPIO_PIN_NONE; 

    return;
}

void  LM3S_USART_Driver::BaudrateBoundary(int ComPortNum, UINT32& maxBaudrateHz, UINT32& minBaudrateHz)
{
    maxBaudrateHz = (SysCtlClockGet()/16);
    minBaudrateHz = 0;
}

BOOL LM3S_USART_Driver::IsBaudrateSupported(int ComPortNum, UINT32 & BaudrateHz)
{
    if (BaudrateHz <= (SysCtlClockGet()/16) )
    {
        return true;
    }
    else
    {
        BaudrateHz = (SysCtlClockGet()/16);
        return false;
    }
}
