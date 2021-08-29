#include "lan91c111.h"
#define extern
#include "tcpip.h"
extern int printf(char *fmt, ...);
static unsigned int HiWord;

// Keil: function added for delays
void delay1ms(int time)
{
	unsigned int dly;

	for(; time; time--)
	{
		for(dly = 2500; dly; dly--)
			;             // wait 1ms
	}
}

// Keil: function added to write PHY   
void output_MDO(int bit_value)
{
	unsigned short val = MGMT_MDOE;

	if(bit_value)
		val |= MGMT_MDO;
	LREG (unsigned short, MGMT) = val;
	LREG (unsigned short, MGMT) = val | MGMT_MCLK;
	LREG (unsigned short, MGMT) = val;
}

// Keil: function added to read PHY   
int input_MDI(void)
{
	int val = 0;

	LREG (unsigned short, MGMT) = 0;
	LREG (unsigned short, MGMT) = MGMT_MCLK;
	if(LREG (unsigned short, MGMT) & MGMT_MDI)
		val = 1;
	LREG (unsigned short, MGMT) = 0;
	return (val);
}

// Keil: function added to write PHY   
void write_PHY(int PhyReg, int Value)
{
	int i;

	LREG (unsigned short, BSR) = 3;
	LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO;

	// 32 consecutive ones on MDO to establish sync
	for(i = 0; i < 32; i++)
	{
		LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO;
		LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO | MGMT_MCLK;
	}
	LREG (unsigned short, MGMT) = MGMT_MDOE;

	// start code (01)
	output_MDO(0);
	output_MDO(1);

	// write command (01)
	output_MDO(0);
	output_MDO(1);

	// write PHY address - which is five 0s for 91C111
	for(i = 0; i < 5; i++)
	{
		output_MDO(0);
	}

	// write the PHY register to write (highest bit first)
	for(i = 0; i < 5; i++)
	{
		output_MDO((PhyReg >> 4) & 0x01);
		PhyReg <<= 1;
	}

	// turnaround MDO
	output_MDO(1);
	output_MDO(0);

	// write the data value (highest bit first)
	for(i = 0; i < 16; i++)
	{
		output_MDO((Value >> 15) & 0x01);
		Value <<= 1;
	}

	// turnaround MDO is tristated
	LREG (unsigned short, MGMT) = 0;
	LREG (unsigned short, MGMT) = MGMT_MCLK;
	LREG (unsigned short, MGMT) = 0;
}

// Keil: function added to read PHY   
unsigned short read_PHY(unsigned char PhyReg)
{
	int i, val;

	LREG (unsigned short, BSR) = 3;
	LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO;

	// 32 consecutive ones on MDO to establish sync
	for(i = 0; i < 32; i++)
	{
		LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO;
		LREG (unsigned short, MGMT) = MGMT_MDOE | MGMT_MDO | MGMT_MCLK;
	}
	LREG (unsigned short, MGMT) = MGMT_MDOE;

	// start code (01)
	output_MDO(0);
	output_MDO(1);

	// read command (10)
	output_MDO(1);
	output_MDO(0);

	// write PHY address - which is five 0s for 91C111
	for(i = 0; i < 5; i++)
	{
		output_MDO(0);
	}

	// write the PHY register to read (highest bit first)
	for(i = 0; i < 5; i++)
	{
		output_MDO((PhyReg >> 4) & 0x01);
		PhyReg <<= 1;
	}

	// turnaround MDO is tristated
	LREG (unsigned short, MGMT) = 0;
	LREG (unsigned short, MGMT) = MGMT_MCLK;
	LREG (unsigned short, MGMT) = 0;

	// read the data value
	val = 0;
	for(i = 0; i < 16; i++)
	{
		val <<= 1;
		val |= input_MDI();
	}

	// turnaround MDO is tristated
	LREG (unsigned short, MGMT) = 0;
	LREG (unsigned short, MGMT) = MGMT_MCLK;
	LREG (unsigned short, MGMT) = 0;

	return (val);
}

// configure port-pins for use with LAN-controller,   
// reset it and send the configuration-sequence   

unsigned char MyMAC[6];

void Init91111(void)
{
// Keil: function modified to access the LAN91C111 on the address/data bus   
// Initializes the chip    
	unsigned short stat, tcr;
	int i;
	printf("init 9111 : ");
	/* Mask off all interrupts */
	LREG (unsigned short, BSR) = 2;
	LREG (unsigned char, MSK) = 0;

	/* Output reset to LAN controller */
	LREG (unsigned short, BSR) = 0;
	LREG (unsigned short, RCR) = RCR_SOFT_RST;
	/* Wait 50 ms for PHY to reset. */
	delay1ms(50);
	LREG (unsigned short, RCR) = 0;
	LREG (unsigned short, TCR) = 0;

	/* Read MAC address stored to external EEPROM */
	LREG (unsigned short, BSR) = 1;
	*(unsigned short*)&MyMAC[0] = LREG(unsigned short, IAR0);
	*(unsigned short*)&MyMAC[2] = LREG(unsigned short, IAR2);
	*(unsigned short*)&MyMAC[4] = LREG(unsigned short, IAR4);

	/* Write Configuration Registers */
	LREG (unsigned short, CR) = CR_EPH_POW_EN | CR_DEFAULT;

	/* Wait 50 ms for MMU operation to finish. */
	delay1ms(50);

	/* Establish the link */

	/* Reset the PHY, timeout is 3 sec */
	write_PHY(0, 0x8000);
	for(i = 0; i < 30; i++)
	{
		delay1ms(100);
		if(!(read_PHY(0) & 0x8000))
		{
			/* reset complete */
			break;
		}
	}

	/* Before auto negotiation, clear phy 18 status */
	read_PHY(18);

	/* Set the MAC Register to Auto Negotiation. */
	LREG (unsigned short, BSR) = 0;
	LREG (unsigned short, RPCR) = RPCR_ANEG | LEDA_10M_100M | LEDB_TX_RX;

	/* Turn off the isolation mode, start Auto_Negotiation process. */
	write_PHY(0, 0x3000);

	/* Wait to complete Auto_Negotiation. */
	for(i = 0; i < 150; i++)
	{
		delay1ms(100);
		stat = read_PHY(1);
		if(stat & 0x0020)
		{
			/* ANEG_ACK set, autonegotiation finished. */
			break;
		}
	}

	/* Check for the output status of the autoneg. */
	for(i = 0; i < 30; i++)
	{
		delay1ms(100);
		stat = read_PHY(18);
		if(!(stat & 0x4000))
		{
			break;
		}
	}

	/* Set the Control Register */
	LREG (unsigned short, BSR) = 1;
	LREG (unsigned short, CTR) = CTR_LE_ENABLE | CTR_CR_ENABLE | CTR_TE_ENABLE |
	CTR_AUTO_REL | CTR_DEFAULT;

	/* Set Receive Control Register. */
	LREG (unsigned short, BSR) = 0;
	LREG (unsigned short, RCR) = RCR_RXEN | RCR_STRIP_CRC;

	/* Setup Transmit Control Register. */
	tcr = TCR_TXENA | TCR_PAD_EN;
	if(stat & 0x0040)
	{
		tcr |= TCR_FDUPLX;
	}
	LREG (unsigned short, TCR) = tcr;

	/* Reset MMU */
	LREG (unsigned short, BSR) = 2;
	LREG (unsigned short, MMUCR) = MMU_RESET;
	while(LREG (unsigned short, MMUCR) & MMUCR_BUSY)
		;
}

unsigned short get_data32(void)
{
	unsigned short retval;
	unsigned int val;

	/* DATA port has only 32-bit access for read. */
	if(HiWord)
	{
		retval = HiWord >> 16;
		HiWord = 0;
	}
	else
	{
		val = LREG(unsigned int, DATA);
		HiWord = val | 1;
		retval = val & 0xFFFF;
	}
	return (retval);
}

// reads a word in little-endian byte order from RX_FRAME_PORT   

unsigned short ReadFrame91111(void)
{
	return (get_data32());
}

// reads a word in big-endian byte order from RX_FRAME_PORT   
// (useful to avoid permanent byte-swapping while reading   
// TCP/IP-data)   

unsigned short ReadFrameBE91111(void)
{
	unsigned short ReturnValue;

	ReturnValue = SwapBytes(get_data32());
	return (ReturnValue);
}

// copies bytes from frame port to MCU-memory   
// NOTES: * an odd number of byte may only be transfered   
//          if the frame is read to the end!   
//        * MCU-memory MUST start at word-boundary   

void CopyFromFrame91111(void *Dest, unsigned short Size)
{
	unsigned short *piDest;                       // Keil: Pointer added to correct expression

	piDest = Dest;                                 // Keil: Line added
	while(Size > 1)
	{
		*piDest++ = ReadFrame91111();
		Size -= 2;
	}

	if(Size)
	{                                         // check for leftover byte...
		*(unsigned char*)piDest = (char)ReadFrame91111();                                         // the LAN-Controller will return 0
	}                                                   // for the highbyte
}

// does a dummy read on frame-I/O-port   
// NOTE: only an even number of bytes is read!   

void DummyReadFrame91111(unsigned short Size)    // discards an EVEN number of bytes   
{                                                // from RX-fifo   
	while(Size > 1)
	{
		ReadFrame91111();
		Size -= 2;
	}
}

// Reads the length of the received ethernet frame and checks if the    
// destination address is a broadcast message or not   
// returns the frame length   
unsigned short StartReadFrame(void)
{
	unsigned short RxLen, State;

	LREG (unsigned short, BSR) = 2;
	State = LREG(unsigned short, FIFO);
	if(State & FIFO_REMPTY)
		return (0);

	/* Read status and packet length */
	LREG (unsigned short, BSR) = 2;
	LREG (unsigned short, PTR) = PTR_RCV | PTR_AUTO_INCR | PTR_READ;

	State = ReadFrame91111();
	RxLen = ReadFrame91111() - 6;

	HiWord = 0;
	return (RxLen);
}

void EndReadFrame(void)
{

	/* MMU free packet. */
	LREG (unsigned short, BSR) = 2;
	LREG (unsigned short, MMUCR) = MMU_REMV_REL_RX;
}

unsigned int CheckFrameReceived(void)
{             // Packet received ?

	LREG (unsigned short, BSR) = 2;         // bank select register
	if(LREG (unsigned char, IST) & IST_RCV)          // more packets received ?
		return (1);
	else
		return (0);
}

// requests space in LAN91C111's on-chip memory for   
// storing an outgoing frame   

void RequestSend(unsigned short FrameSize)
{
	unsigned char packnr;

	LREG (unsigned short, BSR) = 2;
	LREG (unsigned short, MMUCR) = MMU_ALLOC_TX;
	while(!(LREG (unsigned short, IST) & IST_ALLOC_INT))
		;
	packnr = LREG(unsigned char, ARR);
	LREG (unsigned char, PNR) = packnr;
	LREG (unsigned short, PTR) = PTR_AUTO_INCR;

	/* Reserve space for Status */
	LREG (unsigned short, DATA0) = 0x0000;
	LREG (unsigned short, DATA0) = FrameSize + 6;
}

// check if ethernet controller is ready to accept the   
// frame we want to send   

unsigned int Rdy4Tx(void)
{
	return (1);   // the ethernet controller transmits much faster
}               // than the CPU can load its buffers   

// writes a word in little-endian byte order to TX_FRAME_PORT   
void WriteFrame91111(unsigned short Data)
{
	LREG(unsigned short, DATA0) = Data;
}

// copies bytes from MCU-memory to frame port   
// NOTES: * an odd number of byte may only be transfered   
//          if the frame is written to the end!   
//        * MCU-memory MUST start at word-boundary   

void CopyToFrame91111(void *Source, unsigned int Size)
{
	unsigned short *piSource;

	piSource = Source;
	Size = (Size + 1) & 0xFFFE;    // round Size up to next even number
	while(Size > 0)
	{
		WriteFrame91111(*piSource++);
		Size -= 2;
	}

	LREG (unsigned short, DATA0) = RFC_CRC;

	LREG (unsigned short, BSR) = 0;
	LREG (unsigned short, TCR) = TCR_TXENA | TCR_PAD_EN;

	LREG (unsigned short, BSR) = 2;
	LREG (unsigned short, MMUCR) = MMU_ENQ_TX;
}

