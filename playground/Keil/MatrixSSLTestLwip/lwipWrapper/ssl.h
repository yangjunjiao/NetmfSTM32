//*****************************************************************************
//
// ssl.h - SSL layer above the lwIP TCP API. 
//
// Copyright (c) 2007-2008 Luminary Micro, Inc.  All rights reserved.
// 
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 226 of the Stellaris SSL Web Server.
//
//*****************************************************************************
#ifndef _SSL_H_
#define _SSL_H_

//*****************************************************************************
//
// Note: The interface defined here is intended to mirror the lwIP TCP 
//  interface allowing easy porting of other protocols on top of the matrixSSL
//  stack. The subset of APIs supported is based on the requirements of the 
//  existing lwip HTTPD implementation. Addition of any missing APIs required
//  to port additional layers above this is left as an exercise for the reader.
//
//*****************************************************************************
//#include "lwiplib.h"

#include "adapt.h"


//*****************************************************************************
//
// Data type definitions.
//
//*****************************************************************************
struct ssl_pcb;

//*****************************************************************************
//
// Receive and transmit buffer sizes used for SSL sessions. 
//
//*****************************************************************************
#define SSL_RXBUFFER_SIZE 1024
#define SSL_TXBUFFER_SIZE 1024
#define SSL_ENCBUFFER_SIZE 1024

//*****************************************************************************
//
// Public API prototypes.
//
//*****************************************************************************
err_t ssl_init(void);
struct ssl_pcb *ssl_new(void);
err_t ssl_bind(struct ssl_pcb *pcb, struct ip_addr *ipaddr, u16_t port);
err_t ssl_listen(struct ssl_pcb *pcb);
void ssl_arg(struct ssl_pcb *pcb, void *arg);
void ssl_accept(struct ssl_pcb *pcb,
    err_t (* accept)(void *arg, struct ssl_pcb *newpcb, err_t err));
void ssl_recv(struct ssl_pcb *pcb,
    err_t (* recv)(void *arg, struct ssl_pcb *tpcb, unsigned char *p, int len));
void ssl_sent(struct ssl_pcb *pcb,
    err_t (* sent)(void *arg, struct ssl_pcb *tpcb, u16_t len));
void ssl_poll(struct ssl_pcb *pcb,
    err_t (* poll)(void *arg, struct ssl_pcb *tpcb));
void ssl_err(struct ssl_pcb *pcb, void (* err)(void *arg, err_t err));
void ssl_recved(struct ssl_pcb *pcb, u16_t len);
void ssl_abort(struct ssl_pcb *pcb);
err_t ssl_close(struct ssl_pcb *pcb);
err_t ssl_write(struct ssl_pcb *pcb, const void *dataptr, u16_t len);
void ssl_setprio(struct ssl_pcb *pcb, u8_t prio);
err_t ssl_output(struct ssl_pcb *pcb);
u16_t ssl_mss(struct ssl_pcb *pcb);
u16_t ssl_sndbuf(struct ssl_pcb *pcb);

#endif /* _SSL_H_ */
