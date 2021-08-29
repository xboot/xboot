#ifndef __TCPIP_H
#define __TCPIP_H

// easyWEB-stack definitions
#define MYIP_1               10                 // our internet protocol (IP) address
#define MYIP_2               0
#define MYIP_3               2
#define MYIP_4               15

#define SUBMASK_1            255                 // subnet mask
#define SUBMASK_2            255
#define SUBMASK_3            255
#define SUBMASK_4            0

#define GWIP_1               10                 // standard gateway (used if remote
#define GWIP_2               0                 // IP is no part of our subnet)
#define GWIP_3               2
#define GWIP_4               2

#define RETRY_TIMEOUT        8                   // wait max. 8 x 262ms for an ACK (about 2 sec.)
#define FIN_TIMEOUT          2                   // max. time to wait for an ACK of a FIN
// before closing TCP state-machine (about 0.5 s)
#define MAX_RETRYS           4                   // nr. of resendings before reset conn.
// total nr. of transmissions = MAX_RETRYS + 1

#define MAX_TCP_TX_DATA_SIZE 512                 // max. outgoing TCP data size (even!)
#define MAX_TCP_RX_DATA_SIZE 256                 // max. incoming TCP data size (even!)
// (increasing the buffer-size dramatically
// increases the transfer-speed!)

#define MAX_ETH_TX_DATA_SIZE 60                  // 2nd buffer, used for ARP, ICMP, TCP (even!)
// enough to echo 32 byte via ICMP

#define DEFAULT_TTL          64                  // Time To Live sent with packets

// Ethernet network layer definitions
#define ETH_DA_OFS           0                   // Destination MAC address (48 Bit)
#define ETH_SA_OFS           6                   // Source MAC address (48 Bit)
#define ETH_TYPE_OFS         12                  // Type field (16 Bit)
#define ETH_DATA_OFS         14                  // Frame Data
#define ETH_HEADER_SIZE      14

#define FRAME_ARP            0x0806              // frame types (stored in Type/Length field)
#define FRAME_IP             0x0800

// IPv4 layer definitions
#define IP_VER_IHL_TOS_OFS   ETH_DATA_OFS + 0    // Version, Header Length, Type of Service
#define IP_TOTAL_LENGTH_OFS  ETH_DATA_OFS + 2    // IP Frame's Total Length
#define IP_IDENT_OFS         ETH_DATA_OFS + 4    // Identifying Value
#define IP_FLAGS_FRAG_OFS    ETH_DATA_OFS + 6    // Flags and Fragment Offset
#define IP_TTL_PROT_OFS      ETH_DATA_OFS + 8    // Frame's Time to Live, Protocol
#define IP_HEAD_CHKSUM_OFS   ETH_DATA_OFS + 10   // IP Frame's Header Checksum
#define IP_SOURCE_OFS        ETH_DATA_OFS + 12   // Source Address (32 Bit)
#define IP_DESTINATION_OFS   ETH_DATA_OFS + 16   // Destination Address (32 Bit)
#define IP_DATA_OFS          ETH_DATA_OFS + 20   // Frame Data (if no options)
#define IP_HEADER_SIZE       20                  // w/o options

#define IP_VER_IHL           0x4500              // IPv4, Header Length = 5x32 bit
#define IP_TOS_D             0x0010              // TOS low delay
#define IP_TOS_T             0x0008              // TOS high throughput
#define IP_TOS_R             0x0004              // TOS high reliability

#define IP_FLAG_DONTFRAG     0x4000              // don't fragment IP frame
#define IP_FLAG_MOREFRAG     0x2000              // more fragments available
#define IP_FRAGOFS_MASK      0x1FFF              // indicates where this fragment belongs

#define PROT_ICMP            1                   // Internet Control Message Protocol
#define PROT_TCP             6                   // Transmission Control Protocol
#define PROT_UDP             17                  // User Datagram Protocol

// ARP definitions
#define ARP_HARDW_OFS        ETH_DATA_OFS + 0    // Hardware address type
#define ARP_PROT_OFS         ETH_DATA_OFS + 2    // Protocol
#define ARP_HLEN_PLEN_OFS    ETH_DATA_OFS + 4    // byte length of each hardw. / prot. address
#define ARP_OPCODE_OFS       ETH_DATA_OFS + 6    // Opcode
#define ARP_SENDER_HA_OFS    ETH_DATA_OFS + 8    // Hardw. address of sender of this packet
#define ARP_SENDER_IP_OFS    ETH_DATA_OFS + 14   // IP address of sender
#define ARP_TARGET_HA_OFS    ETH_DATA_OFS + 18   // Hardw. address of target of this packet
#define ARP_TARGET_IP_OFS    ETH_DATA_OFS + 24   // IP address of target
#define ARP_FRAME_SIZE       28

#define HARDW_ETH10          1                   // hardware-type 10Mbps Ethernet
#define IP_HLEN_PLEN         0x0604              // MAC = 6 byte long, IP = 4 byte long
#define OP_ARP_REQUEST       1                   // operations for ARP-frames
#define OP_ARP_ANSWER        2

// ICMP definitions
#define ICMP_TYPE_CODE_OFS   IP_DATA_OFS + 0     // type of message
#define ICMP_CHKSUM_OFS      IP_DATA_OFS + 2     // checksum of ICMP-message (16 bit)
#define ICMP_DATA_OFS        IP_DATA_OFS + 4
#define ICMP_HEADER_SIZE     4

#define ICMP_ECHO            8                   // message is an echo request
#define ICMP_ECHO_REPLY      0                   // message is an echo reply

// TCP layer definitions
#define TCP_SRCPORT_OFS      IP_DATA_OFS + 0     // Source Port (16 bit)
#define TCP_DESTPORT_OFS     IP_DATA_OFS + 2     // Destination Port (16 bit)
#define TCP_SEQNR_OFS        IP_DATA_OFS + 4     // Sequence Number (32 bit)
#define TCP_ACKNR_OFS        IP_DATA_OFS + 8     // Acknowledge Number (32 bit)
#define TCP_DATA_CODE_OFS    IP_DATA_OFS + 12    // Data Offset and Control Bits (16 bit)
#define TCP_WINDOW_OFS       IP_DATA_OFS + 14    // Window Size (16 bit)
#define TCP_CHKSUM_OFS       IP_DATA_OFS + 16    // Checksum Field (16 bit)
#define TCP_URGENT_OFS       IP_DATA_OFS + 18    // Urgent Pointer (16 bit)
#define TCP_DATA_OFS         IP_DATA_OFS + 20    // Frame Data (if no options)
#define TCP_HEADER_SIZE      20                  // size w/o options

#define DATA_OFS_MASK        0xF000              // number of 32 bit words in the TCP Header

#define TCP_CODE_FIN         0x0001
#define TCP_CODE_SYN         0x0002
#define TCP_CODE_RST         0x0004
#define TCP_CODE_PSH         0x0008
#define TCP_CODE_ACK         0x0010
#define TCP_CODE_URG         0x0020

#define TCP_OPT_MSS          0x0204              // Type 2, Option Length 4 (Max. Segment Size)
#define TCP_OPT_MSS_SIZE     4

// define some TCP standard-ports, useful for testing...
#define TCP_PORT_ECHO        7                   // echo
#define TCP_PORT_DISCARD     9                   // discard
#define TCP_PORT_DAYTIME     13                  // daytime
#define TCP_PORT_QOTD        17                  // quote of the day
#define TCP_PORT_CHARGEN     19                  // character generator
#define TCP_PORT_HTTP        80                  // word wide web HTTP

// macros
#define SWAPB(Word)          ((unsigned short)((Word) << 8) | ((Word) >> 8))
// convert little <-> big endian

// typedefs
typedef enum
{                                   // states of the TCP-state machine
	CLOSED,                                        // according to RFC793
	LISTENING,
	SYN_SENT,
	SYN_RECD,
	ESTABLISHED,
	FIN_WAIT_1,
	FIN_WAIT_2,
	CLOSE_WAIT,
	CLOSING,
	LAST_ACK,
	TIME_WAIT
} TTCPStateMachine;

typedef enum
{                                   // type of last frame sent. used
	ARP_REQUEST,                                   // for retransmissions
	TCP_SYN_FRAME,
	TCP_SYN_ACK_FRAME,
	TCP_FIN_FRAME,
	TCP_DATA_FRAME
} TLastFrameSent;

// easyWEB's internal variables
extern TTCPStateMachine TCPStateMachine;         // perhaps the most important var at all ;-)
extern TLastFrameSent LastFrameSent;             // retransmission type

extern unsigned short ISNGenHigh;                // upper word of our Initial Sequence Number
extern unsigned long TCPSeqNr;                   // next sequence number to send
extern unsigned long TCPUNASeqNr;                // last unaknowledged sequence number
                                                 // incremented AFTER sending data
extern unsigned long TCPAckNr;                   // next seq to receive and ack to send
                                                 // incremented AFTER receiving data
extern unsigned char TCPTimer;                   // inc'd each 262ms
extern unsigned char RetryCounter;               // nr. of retransmissions

// properties of the just received frame
extern unsigned short RecdFrameLength;           // LAN91C111 reported frame length
extern unsigned short RecdFrameMAC[3];           // 48 bit MAC
extern unsigned short RecdFrameIP[2];            // 32 bit IP
extern unsigned short RecdIPFrameLength;         // 16 bit IP packet length

// the next 3 buffers must be word-aligned!
// (here the 'RecdIPFrameLength' above does that)
extern unsigned short _TxFrame1[(ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE + MAX_TCP_TX_DATA_SIZE) / 2];
extern unsigned short _TxFrame2[(ETH_HEADER_SIZE + MAX_ETH_TX_DATA_SIZE) / 2];
extern unsigned short _RxTCPBuffer[MAX_TCP_RX_DATA_SIZE / 2]; // space for incoming TCP-data
#define TxFrame1      ((unsigned char *)_TxFrame1)
#define TxFrame2      ((unsigned char *)_TxFrame2)
#define RxTCPBuffer   ((unsigned char *)_RxTCPBuffer)

extern unsigned short TxFrame1Size;              // bytes to send in TxFrame1
extern unsigned char TxFrame2Size;               // bytes to send in TxFrame2

extern unsigned char TransmitControl;
#define SEND_FRAME1                    0x01
#define SEND_FRAME2                    0x02

extern unsigned char TCPFlags;
#define TCP_ACTIVE_OPEN                0x01      // easyWEB shall initiate a connection
#define IP_ADDR_RESOLVED               0x02      // IP sucessfully resolved to MAC
#define TCP_TIMER_RUNNING              0x04
#define TIMER_TYPE_RETRY               0x08
#define TCP_CLOSE_REQUESTED            0x10

// prototypes
void DoNetworkStuff(void);

// Handlers for incoming frames
void ProcessEthBroadcastFrame(void);
void ProcessEthIAFrame(void);
void ProcessICMPFrame(void);
void ProcessTCPFrame(void);

// fill TX-buffers
void PrepareARP_REQUEST(void);
void PrepareARP_ANSWER(void);
void PrepareICMP_ECHO_REPLY(void);
void PrepareTCP_FRAME(unsigned short TCPCode);
void PrepareTCP_DATA_FRAME(void);

// general help functions
void SendFrame1(void);
void SendFrame2(void);
void TCPStartRetryTimer(void);
void TCPStartTimeWaitTimer(void);
void TCPRestartTimer(void);
void TCPStopTimer(void);
void TCPHandleRetransmission(void);
void TCPHandleTimeout(void);
unsigned short CalcChecksum(void *Start, unsigned short Count, unsigned char IsTCP);

// functions to work with big-endian numbers
unsigned short SwapBytes(unsigned short Data);
void WriteWBE(unsigned char *Add, unsigned short Data);
void WriteDWBE(unsigned char *Add, unsigned long Data);

// easyWEB-API functions
void TCPLowLevelInit(void);                      // setup timer, LAN-controller, flags...
void TCPPassiveOpen(void);                       // listen for a connection
void TCPActiveOpen(void);                        // open connection
void TCPClose(void);                             // close connection
void TCPReleaseRxBuffer(void);                   // indicate to discard rec'd packet
void TCPTransmitTxBuffer(void);                  // initiate transfer after TxBuffer is filled
void TCPClockHandler(void);                // Keil: interrupt service routine for timer 0

// easyWEB-API global vars and flags
extern unsigned short TCPRxDataCount;            // nr. of bytes rec'd
extern unsigned short TCPTxDataCount;            // nr. of bytes to send

extern unsigned short TCPLocalPort;              // TCP ports
extern unsigned short TCPRemotePort;

extern unsigned short RemoteMAC[3];              // MAC and IP of current TCP-session
extern unsigned short RemoteIP[2];

extern unsigned char SocketStatus;
#define SOCK_ACTIVE                    0x01      // state machine NOT closed
#define SOCK_CONNECTED                 0x02      // user may send & receive data
#define SOCK_DATA_AVAILABLE            0x04      // new data available
#define SOCK_TX_BUF_RELEASED           0x08      // user may fill buffer

#define SOCK_ERROR_MASK                0xF0      // bit-mask to check for errors
#define SOCK_ERR_OK                    0x00      // no error
#define SOCK_ERR_ARP_TIMEOUT           0x10      // timeout waiting for an ARP-REPLY
#define SOCK_ERR_TCP_TIMEOUT           0x20      // timeout waiting for an ACK
#define SOCK_ERR_CONN_RESET            0x30      // connection was reset by the other TCP
#define SOCK_ERR_REMOTE                0x40      // remote TCP caused fatal error
#define SOCK_ERR_ETHERNET              0x50      // network interface error (timeout)

// easyWEB-API buffer-pointers
#define TCP_TX_BUF      ((unsigned char *)TxFrame1 + ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE)
#define TCP_RX_BUF      ((unsigned char *)RxTCPBuffer)

#endif

