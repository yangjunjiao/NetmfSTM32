#******************************************************************************
#
# Makefile - Rules for building the SSL web server using lwIP and MatrixSSL.
#
# Copyright (c) 2007-2008 Luminary Micro, Inc.  All rights reserved.
# 
# Software License Agreement
# 
# Luminary Micro, Inc. (LMI) is supplying this software for use solely and
# exclusively on LMI's microcontroller products.
# 
# The software is owned by LMI and/or its suppliers, and is protected under
# applicable copyright laws.  All rights are reserved.  You may not combine
# this software with "viral" open-source software in order to form a larger
# program.  Any use in violation of the foregoing restrictions may subject
# the user to criminal sanctions under applicable laws, as well as to civil
# liability for the breach of the terms and conditions of this license.
# 
# THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
# OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
# LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
# CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 226 of the Stellaris SSL Web Server.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S6965

#
# The base directory for DriverLib.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../../../third_party/bget
VPATH+=../../../boards/ek-lm3s6965
VPATH+=../../../boards/ek-lm3s6965
VPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open/src/os/no_os
VPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open/src/os
VPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open/src/crypto/peersec
VPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open/src/pki
VPATH+=../../../third_party/fatfs/port
VPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open/src/
VPATH+=../../../third_party/fatfs/src
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=.
IPATH+=../../..
IPATH+=../../../driverlib
IPATH+=../../../boards/ek-lm3s6965
IPATH+=../../../boards/ek-lm3s6965
IPATH+=../../../third_party/lwip-1.3.0/apps/httpserver_raw
IPATH+=../../../third_party/lwip-1.3.0/ports/stellaris/include
IPATH+=../../../third_party/lwip-1.3.0/src/include
IPATH+=../../../third_party/lwip-1.3.0/src/include/ipv4
IPATH+=../../../../MatrixSSL/matrixssl-1-8-6-open
IPATH+=../../../third_party/fatfs/src
IPATH+=../../../utils

#
# The default rule, which causes the SSL web server using lwIP and MatrixSSL to be built.
#
all: ${COMPILER}
all: ${COMPILER}/webserver-ssl-ek-lm3s6965.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir ${COMPILER}

#
# Rules for building the SSL web server using lwIP and MatrixSSL.
#
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/arc4.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/asn1.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/base64.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/bget.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/certSrv.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/cipherSuite.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/debug.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/des3.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/display.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/ff.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/httpsd.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/lmi_fs.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/lwiplib.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/matrixSsl.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/md2.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/md5.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/mmc-ek-lm3s6965.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/mpi.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/no_os.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/osram128x64x4.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/privkeySrv.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/rsa.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/rsaPki.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/sha1.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/ssl.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/sslDecode.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/sslEncode.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/sslv3.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/uartstdio.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/ustdlib.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/webserver-ssl.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${COMPILER}/x509.o
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: ${ROOT}/driverlib/${COMPILER}/libdriver.a
ifeq (${COMPILER}, gcc)
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: webserver-ssl-ek-lm3s6965.ld
endif
SCATTERgcc_webserver-ssl-ek-lm3s6965=webserver-ssl-ek-lm3s6965.ld
ifeq (${COMPILER}, sourcerygxx)
${COMPILER}/webserver-ssl-ek-lm3s6965.axf: webserver-ssl-ek-lm3s6965_sourcerygxx.ld
endif
SCATTERsourcerygxx_webserver-ssl-ek-lm3s6965=lm3s6965-rom.ld -T webserver-ssl-ek-lm3s6965_sourcerygxx.ld
ENTRY_webserver-ssl-ek-lm3s6965=ResetISR
CFLAGSgcc=-DLUMINARY_MICRO
CFLAGSsourcerygxx=-DLUMINARY_MICRO

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
