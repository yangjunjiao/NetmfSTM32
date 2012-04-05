/******************************************************************************
 *
 * webserver-ssl-ek-lm3s8962_codered.ld - Code Red linker configuration file for webserver-ssl-ek-lm3s8962.
 *
 * Copyright (c) 2007 Luminary Micro, Inc.  All rights reserved.
 * Luminary Micro Confidential - For Use Under NDA Only
 *
 *****************************************************************************/

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00040000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00010000
}

SECTIONS
{
    _vRamTop = 0x20000000 + 0x00010000;
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
    } > FLASH

    /*
     * for exception handling/unwind - some Newlib functions (in common with
     * C++ and STDC++) use this.
     */

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH

    _etext = .;

    .data : AT (__exidx_end)
    {
        _data = .;
        *(vtable)
        *(.data*)
        _edata = .;
    } > SRAM

    /* zero initialized data */

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    /* Where we put the heap with cr_clib */

    .cr_heap :
    {
        end = .;
        _pvHeapStart = .;
    } > SRAM

    /*
     * Note: (ref: M0000066)
     * Moving the stack down by 16 is to work around a GDB bug.
     * This space can be reclaimed for Production Builds.
     */

    _vStackTop = _vRamTop - 16;
}
