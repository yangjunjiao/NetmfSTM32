////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for the GadgeteerETH board (STM32F2): Copyright (c) Oberon microsystems, Inc.
//
//  *** GadgeteerETH Board specific IO Port Initialization ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\STM32F2\DeviceCode\stm32f2xx.h"



void __section(SectionForBootstrapOperations) BootstrapCode_GPIO() {

    /* Enable GPIO clocks */  
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN
                  | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN
                  | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN;

    // psc 2010.10.26: pin not needed for ETH board
    //CPU_GPIO_EnableOutputPin(3 * 16 + 7,  FALSE); // PD7 = LED1
    CPU_GPIO_EnableOutputPin(1 * 16 + 15, FALSE); // PB15 = LED2

}
