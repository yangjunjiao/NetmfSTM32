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
#include <string.h>
#include <tinyhal.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#define MAX_INT_NUMBER 71
static BOOL IntEnableStateTab[MAX_INT_NUMBER];

//--//
void __irq IRQ_Handler()
{
}

void CPU_INTC_Initialize()
{
	memset(IntEnableStateTab,0, sizeof(IntEnableStateTab));
    IntMasterEnable();
}

BOOL CPU_INTC_ActivateInterrupt( UINT32 Irq_Index, HAL_CALLBACK_FPN ISR, void* ISR_Param )
{
  if(ISR != NULL)
	{
		IntRegister(Irq_Index, (void (*)(void))ISR);
		IntEnableStateTab[Irq_Index] = TRUE;
		IntEnable(Irq_Index);
		return TRUE;
	}
	else
	{
		return FALSE;	
	}
}

BOOL CPU_INTC_DeactivateInterrupt( UINT32 Irq_Index )
{
    IntUnregister(Irq_Index);
    IntDisable(Irq_Index);
    IntEnableStateTab[Irq_Index] = FALSE;
    return TRUE;
}

BOOL CPU_INTC_InterruptEnable( UINT32 Irq_Index )
{
    IntEnableStateTab[Irq_Index] = TRUE;
    IntEnable(Irq_Index);
	return TRUE;
}

BOOL CPU_INTC_InterruptDisable( UINT32 Irq_Index )
{
    IntDisable(Irq_Index);
    IntEnableStateTab[Irq_Index] = FALSE;
	return TRUE;
}

BOOL CPU_INTC_InterruptEnableState( UINT32 Irq_Index )
{
    return IntEnableStateTab[Irq_Index];
}

BOOL CPU_INTC_InterruptState( UINT32 Irq_Index )
{
    return FALSE;
}


