#ifndef __LWIP_LWIPOPTS_H__
#define __LWIP_LWIPOPTS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
   -----------------------------------------------
   ---------- Platform specific locking ----------
   -----------------------------------------------
*/

/** 
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                          1

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT            0

/*
   ------------------------------------
   ---------- Memory options ----------
   ------------------------------------
*/
/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#define MEM_SIZE                        16000


/*
   ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------
*/
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                   30

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB                4

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                4

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                2

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         8

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_SEG                16

/**
 * MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#define MEMP_NUM_ARP_QUEUE              2

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 *
 * To this default value, 1 was added for the snmp_increment timer.
 */
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0)) + 1

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETBUF                 0

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN                0

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication. 
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_API          0

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets. 
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_INPKT        0

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool. 
 */
#define PBUF_POOL_SIZE                  32

/*
   ---------------------------------
   ---------- ARP options ----------
   ---------------------------------
*/
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#define LWIP_ARP                        1

/*
   --------------------------------
   ---------- IP options ----------
   --------------------------------
*/
/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#define IP_FORWARD                      1

/**
 * IP_OPTIONS: Defines the behavior for IP options.
 *      IP_OPTIONS==0_ALLOWED: All packets with IP options are dropped.
 *      IP_OPTIONS==1_ALLOWED: IP options are allowed (but not parsed).
 */
#define IP_OPTIONS_ALLOWED              1

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY                   1

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG                         1

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#define IP_REASS_MAX_PBUFS              10

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented.
 */
#define IP_FRAG_USES_STATIC_BUF         0

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL                  255

/*
   ----------------------------------
   ---------- ICMP options ----------
   ----------------------------------
*/
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP                       1

/**
 * ICMP_TTL: Default value for Time-To-Live used by ICMP packets.
 */
#define ICMP_TTL                       (IP_DEFAULT_TTL)

/*
   ---------------------------------
   ---------- RAW options ----------
   ---------------------------------
*/
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                        1

/*
   ----------------------------------
   ---------- DHCP options ----------
   ----------------------------------
*/
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                       1

/*
   ------------------------------------
   ---------- AUTOIP options ----------
   ------------------------------------
*/
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#define LWIP_AUTOIP                     0

/*
   ----------------------------------
   ---------- SNMP options ----------
   ----------------------------------
*/
/**
 * LWIP_SNMP==1: Turn on SNMP module. UDP must be available for SNMP
 * transport.
 */
#define LWIP_SNMP                       1
#define LWIP_MIB2_CALLBACKS             0
#define MIB2_STATS                      1

/*
   ----------------------------------
   ---------- IGMP options ----------
   ----------------------------------
*/
/**
 * LWIP_IGMP==1: Turn on IGMP module. 
 */
#define LWIP_IGMP                       0

/*
   ----------------------------------
   ---------- DNS options -----------
   ----------------------------------
*/
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS                        0

/*
   ---------------------------------
   ---------- UDP options ----------
   ---------------------------------
*/
/**
 * LWIP_UDP==1: Turn on UDP.
 */
#define LWIP_UDP                        1

/**
 * LWIP_UDPLITE==1: Turn on UDP-Lite. (Requires LWIP_UDP)
 */
#define LWIP_UDPLITE                    0

/**
 * UDP_TTL: Default Time-To-Live value.
 */
#define UDP_TTL                         (IP_DEFAULT_TTL)

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP                        1

/*
   ----------------------------------
   ---------- Pbuf options ----------
   ----------------------------------
*/
/**
 * PBUF_LINK_HLEN: the number of bytes that should be allocated for a
 * link level header. The default is 14, the standard value for
 * Ethernet.
 */
#define PBUF_LINK_HLEN                  16

/*
   ------------------------------------
   ---------- LOOPIF options ----------
   ------------------------------------
*/
/**
 * LWIP_HAVE_LOOPIF==1: Support loop interface (127.0.0.1) and loopif.c
 */
#define LWIP_HAVE_LOOPIF                1


/*
   ----------------------------------------------
   ---------- Sequential layer options ----------
   ----------------------------------------------
*/

/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    0

/*
   ------------------------------------
   ---------- Socket options ----------
   ------------------------------------
*/
/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     0

/*
   ----------------------------------------
   ---------- Statistics options ----------
   ----------------------------------------
*/
/**
 * LWIP_STATS==1: Enable statistics collection in lwip_stats.
 */
#define LWIP_STATS                      1

#ifdef __cplusplus
}
#endif

#endif /* __LWIP_LWIPOPTS_H__ */
