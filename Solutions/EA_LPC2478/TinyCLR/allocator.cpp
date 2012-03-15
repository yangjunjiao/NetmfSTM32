////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <tinyhal.h>

////////////////////////////////////////////////////////////////////////////////

static void RuntimeFault( const char* szText )
{
    lcd_printf("\014ERROR:\r\n%s\r\n", szText );
    debug_printf( "ERROR: %s\r\n", szText );

    HARD_BREAKPOINT();
}

void *operator new(size_t) 
{
    RuntimeFault( "new(size_t)" );

    return NULL;
}

void *operator new[](size_t)
{
    RuntimeFault( "new[](size_t)" );

    return NULL;
}

void operator delete (void*)
{
    RuntimeFault( "delete(void*)" );
}

void operator delete[] (void*)
{
    RuntimeFault( "delete[](void*)" );
}

////////////////////////////////////////////////////////////////////////////////
