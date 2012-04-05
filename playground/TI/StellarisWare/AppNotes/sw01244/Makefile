#******************************************************************************
#
# Makefile - Rules for building the ${APPNOTE} examples.
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
# A list of the directories containing the examples.
#
DIRS= ek-lm3s6965_revc ek-lm3s6965 ek-lm3s8962

#
# The default rule, which causes the examples to be built.
#
all:
	@for i in ${DIRS};  \
	 do                 \
	     make -C $${i}; \
	 done

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${wildcard *~} __dummy__
	@-for i in ${DIRS};        \
	  do                       \
	      make -C $${i} clean; \
	  done

#
# The rule used to apply the Luminary Micro patches to the MatrixSSL
# source. This assumes that MatrixSSL 1.8.6 has been unzipped into a
# directory called MatrixSSL at the same level as the DriverLib
# directory (that is, the source appears at 
#
#   ../../../MatrixSSL/matrixssl-1-8-6-open/src
#
# relative to the directory containing this Makefile).
#
prep:
	patch -Nup1 -d ../../../MatrixSSL/matrixssl-1-8-6-open -i ../../matrixssl-1-8-6-open.patch

