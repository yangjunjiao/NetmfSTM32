////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for STM32ETH2 board (STM32): Copyright (c) Oberon microsystems, Inc.
//
//  STM32ETH2-specific definitions
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PLATFORM_STM32ETH2_SELECTOR_H_
#define _PLATFORM_STM32ETH2_SELECTOR_H_ 1

/////////////////////////////////////////////////////////
//
// processor and features
//

#if defined(PLATFORM_ARM_STM32ETH2)
#define HAL_SYSTEM_NAME                     "STM32ETH2"

//#define STM32F10X_HD  // STM32F103ZE cpu
#define STM32F10X_XL
#define PLATFORM_ARM_STM32

//#define USB_ALLOW_CONFIGURATION_OVERRIDE  1


#if 1 //defined(iMXS_net_open_SOCKETS_ENABLED)

#define NETWORK_INTERFACE_COUNT             2

#define PLATFORM_DEPENDENT__SOCKETS_MAX_SEND_LENGTH 8192

#define NETWORK_MEMORY_PROFILE__small      1

//#define NETWORK_MEMORY_POOL__INCLUDE_SSL    1
#include <pal\net\Network_Defines_lwip.h>

#define NETWORK_USE_LOOPBACK                1
#define deine LWIP_HAVE_LOOPIF 1
//#define NETWORK_USE_DHCP                    1 //SK only for rtip???

//--//

#define PLATFORM_DEPENDENT__NETWORK_DNS_MIN_DELAY  4 
#define PLATFORM_DEPENDENT__NETWORK_DNS_MAX_DELAY  32
#define PLATFORM_DEPENDENT__NETWORK_DNS_RETRIES    3


#endif


//
// processor and features
//
/////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////
//
// constants
//

#define SYSTEM_CLOCK_HZ                 72000000
#define SYSTEM_CYCLE_CLOCK_HZ           72000000  // 72MHz
#define SYSTEM_APB1_CLOCK_HZ            36000000  // 36MHz
#define SYSTEM_APB2_CLOCK_HZ            72000000  // 72MHz
#define SYSTEM_CRYSTAL_CLOCK_HZ         8000000   // 8MHz external clock

#define CLOCK_COMMON_FACTOR             1000000   // GCD(SYSTEM_CLOCK_HZ, 1M)

#define SLOW_CLOCKS_PER_SECOND          1000000
#define SLOW_CLOCKS_TEN_MHZ_GCD         1000000   // GCD(SLOW_CLOCKS_PER_SECOND, 10M)
#define SLOW_CLOCKS_MILLISECOND_GCD     1000      // GCD(SLOW_CLOCKS_PER_SECOND, 1k)

#define FLASH_MEMORY_Base   0x08000000
#define FLASH_MEMORY_Size   0x00080000
#define SRAM1_MEMORY_Base   0x20000000
#define SRAM1_MEMORY_Size   0x48100000

#define TXPROTECTRESISTOR               RESISTOR_DISABLED
#define RXPROTECTRESISTOR               RESISTOR_DISABLED
#define CTSPROTECTRESISTOR              RESISTOR_DISABLED
#define RTSPROTECTRESISTOR              RESISTOR_DISABLED

#define INSTRUMENTATION_H_GPIO_PIN      GPIO_PIN_NONE

#define TOTAL_USART_PORT                3
#define USART_DEFAULT_PORT              COM2
#define USART_DEFAULT_BAUDRATE          115200

#define DEBUG_TEXT_PORT    USART_DEFAULT_PORT //USB1
#define STDIO              USART_DEFAULT_PORT //USB1
#define DEBUGGER_PORT      USB1
#define MESSAGING_PORT     USB1


#define STM32_USE_I2C2                  1

#define STM32_USB_Attach_Pin_Low        (16 + 14) // B14


#define DRIVER_PAL_BUTTON_MAPPING                  \
    { 6 * 16 + 15, BUTTON_B0 }, /* G15: Up */      \
    { 3 * 16 +  3, BUTTON_B1 }, /* D3:  Down */    \
    { 6 * 16 + 14, BUTTON_B2 }, /* G14: Left */    \
    { 6 * 16 + 13, BUTTON_B3 }, /* G13: Right */   \
    { 6 * 16 +  7, BUTTON_B4 }, /* G7:  Enter */   \
    { 6 * 16 +  8, BUTTON_B5 }, /* G8:  User */

//
// constants
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// global functions
//

//
// global functions
//
/////////////////////////////////////////////////////////

#include <processor_selector.h>

#endif // PLATFORM_ARM_STM32ETH2
//
// drivers
/////////////////////////////////////////////////////////

#endif // _PLATFORM_STM32ETH2_SELECTOR_H_
