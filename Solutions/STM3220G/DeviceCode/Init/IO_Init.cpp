////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
//
//  Copyright (c) Microsoft Corporation. All rights reserved.
//  Implementation for the STM3220G board (STM32F2): Copyright (c) Oberon microsystems, Inc.
//
//  *** STM3220G Board specific IO Port Initialization ***
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <tinyhal.h>
#include "..\..\..\..\DeviceCode\Targets\Native\STM32F4\DeviceCode\stm32f2xx.h"



void __section(SectionForBootstrapOperations) STM3210E_InitSRam() {
/*!< FSMC Bank1 NOR/SRAM3 is used for the STM3210E-EVAL, if another Bank is 
  required, then adjust the Register Addresses */

    /* Enable FSMC clock */
    RCC->AHB3ENR |= RCC_AHB3ENR_FSMCEN;
  
    /* Enable GPIOD, GPIOE, GPIOF and GPIOG clocks */  
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN
                  | RCC_AHB1ENR_GPIOFEN | RCC_AHB1ENR_GPIOGEN;
  
    // Configure Ax, Dx, NOE, NWE, NE3, NBLx pins
  
/*
    // pins:      15------8                 7------0
    GPIOD->CRH = 0xBBBBBBBB; GPIOD->CRL = 0x44BB44BB;
    GPIOE->CRH = 0xBBBBBBBB; GPIOE->CRL = 0xBBBBBBBB;  
    GPIOF->CRH = 0xBBBB4444; GPIOF->CRL = 0x44BBBBBB;  
    GPIOG->CRH = 0x444B4B44; GPIOG->CRL = 0x44BBBBBB;  // also enable NE4 (LCD)
*/
   
    // Enable FSMC Bank1_SRAM 3 (mode A, 16 bit)
  
    FSMC_Bank1->BTCR[4] = FSMC_BCR1_EXTMOD | FSMC_BCR1_WREN | FSMC_BCR1_MWID_0 | FSMC_BCR1_MBKEN;
    FSMC_Bank1->BTCR[5] = 0x00000100; // addset = 0+1, dataset = 1+1
    FSMC_Bank1E->BWTR[4] = 0x00000100; // addset = 0+1, dataset = 1+1

}


void __section(SectionForBootstrapOperations) BootstrapCode_GPIO() {

    /* Enable GPIO clocks */  
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN
                  | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN
                  | RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN | RCC_AHB1ENR_GPIOIEN;

    CPU_GPIO_EnableOutputPin(6 * 16 + 6, FALSE); // PG6 = LED1
    CPU_GPIO_EnableOutputPin(6 * 16 + 8, FALSE); // PG8 = LED2
    CPU_GPIO_EnableOutputPin(8 * 16 + 9, FALSE); // PI9 = LED3
    CPU_GPIO_EnableOutputPin(2 * 16 + 7, FALSE); // PC7 = LED4

    //STM3210E_InitSRam(); // initialize FSMC
}
