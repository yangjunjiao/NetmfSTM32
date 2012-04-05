SSL Web Server

This example application demonstrates the operation of the Stellaris
Ethernet controller using the lwIP TCP/IP Stack and MatrixSSL library.
DHCP is used to obtain an ethernet address.  If DHCP times out without
obtaining an address, an IP address is automatically allocated using the
RFC3927 automatic link-local IP address allocation algorithm. The address
that is selected will be shown on the OLED display.

The file system code will first check to see if an SD card has been plugged
into the microSD slot.  If so, all file requests from the web server will
be directed to the SD card.  Otherwise, a default set of pages served up
by an internal file system will be used.

Requests may be made using HTTPS on port 443 (the default). Unencrypted
HTTP is not supported in this example.

This project file assumes that the MatrixSSL source has been installed in
a directory at the same level as DriverLib. For example, if you installed
DriverLib in c:\DriverLib, the project will expect to find MatrixSSL under
C:\MatrixSSL such that the source files can be found at
C:\MatrixSSL\matrixssl-1-8-3-open\src.

Before building this project, please ensure that you have applied the
supplied patch to the MatrixSSL source to enable Stellaris support. The
patch file is named matrixssl-1-8-3-open.patch and should be applied by
executing the following command from within the
C:\MatrixSSL\matrixssl-1-8-3-open directory:

   patch -Np1 -i <path_to_patch>/matrixssl-1-8-3-open.patch

For additional details on lwIP, refer to the lwIP web page at:
http://www.sics.se/~adam/lwip/

For additional details on MatrixSSL, refer to the web page at:
http://www.matrixssl.org

-------------------------------------------------------------------------------

Copyright (c) 2007-2008 Luminary Micro, Inc.  All rights reserved.
Luminary Micro Confidential - For Use Under NDA Only
