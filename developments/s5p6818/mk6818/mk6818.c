#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLKSIZE						(512)

#define SECBOOT_NSIH_POSITION		(1)
#define SECBOOT_POSITION			(2)
#define BOOTLOADER_NSIH_POSITION	(64)
#define BOOTLOADER_POSITION			(65)

struct nand_bootinfo_t
{
	uint8_t	addrstep;
	uint8_t	tcos;
	uint8_t	tacc;
	uint8_t	toch;
	uint32_t pagesize;
	uint32_t crc32;
};

struct spi_bootinfo_t
{
	uint8_t	addrstep;
	uint8_t	reserved0[3];
	uint32_t reserved1;
	uint32_t crc32;
};

struct sdmmc_bootinfo_t
{
	uint8_t	portnumber;
	uint8_t	reserved0[3];
	uint32_t reserved1;
	uint32_t crc32;
};

struct sdfs_bootinfo_t
{
	char bootfile[12];
};

union device_bootinfo_t
{
	struct nand_bootinfo_t nandbi;
	struct spi_bootinfo_t spibi;
	struct sdmmc_bootinfo_t sdmmcbi;
	struct sdfs_bootinfo_t sdfsbi;
};

struct ddr_initinfo_t
{
	uint8_t	chipnum;
	uint8_t	chiprow;
	uint8_t	buswidth;
	uint8_t	reserved0;

	uint16_t chipmask;
	uint16_t chipbase;

	uint8_t	cwl;
	uint8_t	wl;
	uint8_t	rl;
	uint8_t	ddrrl;

	uint32_t phycon4;
	uint32_t phycon6;

	uint32_t timingaref;
	uint32_t timingrow;
	uint32_t timingdata;
	uint32_t timingpower;
};

struct boot_info_t
{
	uint32_t vector[8];					// 0x000 ~ 0x01C
	uint32_t vector_rel[8];				// 0x020 ~ 0x03C

	uint32_t deviceaddr;				// 0x040
	uint32_t loadsize;					// 0x044
	uint32_t loadaddr;					// 0x048
	uint32_t launchaddr;				// 0x04C

	union device_bootinfo_t dbi;		// 0x050 ~ 0x058

	uint32_t pll[4];					// 0x05C ~ 0x068
	uint32_t pllspread[2];				// 0x06C ~ 0x070
	uint32_t dvo[5];					// 0x074 ~ 0x084

	struct ddr_initinfo_t dii;			// 0x088 ~ 0x0A8

	uint32_t axibottomslot[32];			// 0x0AC ~ 0x128
	uint32_t axidisplayslot[32];		// 0x12C ~ 0x1A8

	uint32_t stub[(0x1F8 - 0x1A8) / 4];	// 0x1AC ~ 0x1F8
	uint32_t signature;					// 0x1FC "NSIH"
};

static int process_nsih(const char * filename, unsigned char * outdata)
{
	FILE * fp;
	char ch;
	int writesize, skipline, line, bytesize, i;
	unsigned int writeval;

	fp = fopen(filename, "r+b");
	if(!fp)
	{
		printf("Failed to open %s file.\n", filename);
		return 0;
	}

	bytesize = 0;
	writeval = 0;
	writesize = 0;
	skipline = 0;
	line = 0;

	while(0 == feof(fp))
	{
		ch = fgetc (fp);

		if (skipline == 0)
		{
			if (ch >= '0' && ch <= '9')
			{
				writeval = writeval * 16 + ch - '0';
				writesize += 4;
			}
			else if (ch >= 'a' && ch <= 'f')
			{
				writeval = writeval * 16 + ch - 'a' + 10;
				writesize += 4;
			}
			else if (ch >= 'A' && ch <= 'F')
			{
				writeval = writeval * 16 + ch - 'A' + 10;
				writesize += 4;
			}
			else
			{
				if(writesize == 8 || writesize == 16 || writesize == 32)
				{
					for(i=0 ; i<writesize/8 ; i++)
					{
						outdata[bytesize++] = (unsigned char)(writeval & 0xFF);
						writeval >>= 8;
					}
				}
				else
				{
					if (writesize != 0)
						printf("Error at %d line.\n", line + 1);
				}

				writesize = 0;
				skipline = 1;
			}
		}

		if(ch == '\n')
		{
			line++;
			skipline = 0;
			writeval = 0;
		}
	}

	printf("NSIH : %d line processed.\n", line + 1);
	printf("NSIH : %d bytes generated.\n", bytesize);

	fclose(fp);
	return bytesize;
}

int main(int argc, char *argv[])
{
	FILE * fp;
	struct boot_info_t * bi;
	unsigned char nsih[512];
	char * buffer;
	int length, reallen;
	int nbytes, filelen;

	if(argc != 5)
	{
		printf("Usage: mk6818 <destination> <nsih> <2ndboot> <bootloader>\n");
		return -1;
	}

	if(process_nsih(argv[2], &nsih[0]) != 512)
		return -1;

	length = 32 * 1024 * 1024;
	buffer = malloc(length);
	memset(buffer, 0, length);

	/* 2ndboot nsih */
	memcpy(&buffer[(SECBOOT_NSIH_POSITION - 1) * BLKSIZE], &nsih[0], 512);

	/* 2ndboot */
	fp = fopen(argv[3], "r+b");
	if(fp == NULL)
	{
		printf("Open file 2ndboot error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[(SECBOOT_POSITION - 1) * BLKSIZE], 1, filelen, fp);
	if(nbytes != filelen)
	{
		printf("Read file 2ndboot error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* fix 2ndboot nsih */
	bi = (struct boot_info_t *)(&buffer[(SECBOOT_NSIH_POSITION - 1) * BLKSIZE]);
	/* ... */

	/* bootloader nsih */
	memcpy(&buffer[(BOOTLOADER_NSIH_POSITION - 1) * BLKSIZE], &nsih[0], 512);

	/* bootloader */
	fp = fopen(argv[4], "r+b");
	if(fp == NULL)
	{
		printf("Open file bootloader error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	filelen = ftell(fp);
	reallen = (BOOTLOADER_POSITION - 1) * BLKSIZE + filelen;
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[(BOOTLOADER_POSITION - 1) * BLKSIZE], 1, filelen, fp);
	if(nbytes != filelen)
	{
		printf("Read file bootloader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* fix bootloader nsih */
	bi = (struct boot_info_t *)(&buffer[(BOOTLOADER_NSIH_POSITION - 1) * BLKSIZE]);
	bi->deviceaddr = 0x00008000;
	bi->loadsize = ((filelen + 512 + 512) >> 9) << 9;
	bi->loadaddr = 0x43C00000;
	bi->launchaddr = 0x43C00000;

	/* destination */
	fp = fopen(argv[1], "w+b");
	if(fp == NULL)
	{
		printf("Destination file open error\n");
		free(buffer);
		return -1;
	}

	nbytes = fwrite(buffer, 1, reallen, fp);
	if(nbytes != reallen)
	{
		printf("Destination file write error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}

	free(buffer);
	fclose(fp);

	printf("Generate destination file: %s\n", argv[1]);

	return 0;
}
