//-----------------------------------------------------------------------------
// Software that is described herein is for illustrative purposes only  
// which provides customers with programming information regarding the  
// products. This software is supplied "AS IS" without any warranties.  
// NXP Semiconductors assumes no responsibility or liability for the
// use of the software, conveys no license or title under any patent, 
// copyright, or mask work right to the product. NXP Semiconductors 
// reserves the right to make changes in the software without 
// notification. NXP Semiconductors also make no representation or 
// warranty that such application will be suitable for the specified 
// use without further testing or modification. 
//-----------------------------------------------------------------------------
// PSC 2011.10.11: added for lwip support. Included by
// \DeviceCode\pal\net\Network_defines_lwip.h
// Warning: this file is not recompiled if no other file is modified!
// Clean before compiling
//-----------------------------------------------------------------------------

#define TCPIP_LWIP                                  1

// Custom memory profile
#define NETWORK_MEMORY_PROFILE_LWIP__custom         1

#define MEM_SIZE__custom                            (8*1024)
#define MEMP_NUM_PBUF__custom                       16
#define MEMP_NUM_UDP_PCB__custom                    6
#define MEMP_NUM_TCP_PCB__custom                    5
#define MEMP_NUM_TCP_PCB_LISTEN__custom             5
#define MEMP_NUM_TCP_SEG__custom                    32
#define MEMP_NUM_SYS_TIMEOUT__custom                8
#define MEMP_NUM_NETBUF__custom                     8
#define MEMP_NUM_NETCONN__custom                    10
#define PBUF_POOL_SIZE__custom                      12
#define PBUF_POOL_BUFSIZE__custom                   256
#define TCP_MSS__custom                             128
#define TCP_SND_BUF__custom                         (2*TCP_MSS)
#define TCP_SND_QUEUELEN__custom                    (4*TCP_SND_BUF/TCP_MSS)
#define TCP_WND__custom                             1024
#define TCP_SNDLOWAT__custom                        (TCP_SND_BUF/2)

// Custom lwIP options
#define IP_FORWARD__custom              0
#define DHCP_DOES_ARP_CHECK__custom     1
#define LWIP_IGMP__custom               1