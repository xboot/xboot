
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

#define GPFSEL1 0x3f200004
#define GPSET0  0x3f20001C
#define GPCLR0  0x3f200028
#define GPPUD       0x3f200094
#define GPPUDCLK0   0x3f200098

#define AUX_ENABLES     0x3f215004
#define AUX_MU_IO_REG   0x3f215040
#define AUX_MU_IER_REG  0x3f215044
#define AUX_MU_IIR_REG  0x3f215048
#define AUX_MU_LCR_REG  0x3f21504C
#define AUX_MU_MCR_REG  0x3f215050
#define AUX_MU_LSR_REG  0x3f215054
#define AUX_MU_MSR_REG  0x3f215058
#define AUX_MU_SCRATCH  0x3f21505C
#define AUX_MU_CNTL_REG 0x3f215060
#define AUX_MU_STAT_REG 0x3f215064
#define AUX_MU_BAUD_REG 0x3f215068

//GPIO14  TXD0 and TXD1
//GPIO15  RXD0 and RXD1
//alt function 5 for uart1
//alt function 0 for uart0

//((250,000,000/115200)/8)-1 = 270

int notmain(void)
{
    unsigned int ra;
    int i = 100;

    PUT32(AUX_ENABLES,1);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_CNTL_REG,0);
    PUT32(AUX_MU_LCR_REG,3);
    PUT32(AUX_MU_MCR_REG,0);
    PUT32(AUX_MU_IER_REG,0);
    PUT32(AUX_MU_IIR_REG,0xC6);
    PUT32(AUX_MU_BAUD_REG,270);

    ra=GET32(GPFSEL1);
    ra&=~(7<<12); //gpio14
    ra|=2<<12;    //alt5
    PUT32(GPFSEL1,ra);

    PUT32(GPPUD,0);
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,(1<<14));
    for(ra=0;ra<150;ra++) dummy(ra);
    PUT32(GPPUDCLK0,0);

    PUT32(AUX_MU_CNTL_REG,2);

    while(i-- > 0)
    {
        while(1)
        {
            if(GET32(AUX_MU_LSR_REG)&0x20) break;
        }
        PUT32(AUX_MU_IO_REG, 0x30 + (i & 7));
    }

    return(0);
}
