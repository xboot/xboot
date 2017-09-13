#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLKSIZE						(512)
#define ID_BLOCK_POSITION			(64)
#define LOADER_BLOCK_POSITION		(68)
#define LOADER_BLOCK_SIZE			(12)
#define BOOTLOADER_BLOCK_POSITION	(80)

struct idblock0_t {
	uint32_t sig;
	uint8_t	 reserved0[4];
	uint32_t rc4_flag;
	uint16_t bootcode1_offset;
	uint16_t bootcode2_offset;
	uint8_t  reserved1[490];
	uint16_t flash_data_size;
	uint16_t flash_boot_size;
	uint8_t  reserved2[2];
};

struct idblock1_t {
	uint16_t sys_reserved_block;
	uint16_t disk0_size;
	uint16_t disk1_size;
	uint16_t disk2_size;
	uint16_t disk3_size;
	uint32_t chip_tag;
	uint32_t machine_id;
	uint16_t loader_year;
	uint16_t loader_date;
	uint16_t loader_ver;
	uint8_t  reserved1[72];
	uint16_t flash_data_offset;
	uint16_t flash_data_len;
	uint8_t  reserved2[384];
	uint32_t flash_chip_size;
	uint8_t  reserved3;
	uint8_t  access_time;
	uint16_t phy_block_size;
	uint8_t  phy_page_size;
	uint8_t  ecc_bits;

	uint8_t  reserved4[8];
	uint16_t id_block0;
	uint16_t id_block1;
	uint16_t id_block2;
	uint16_t id_block3;
	uint16_t id_block4;
};

struct idblock2_t {
	uint16_t chip_info_size;
	uint8_t chip_info[510];
};

struct idblock3_t {
	uint16_t sn_size;
	uint8_t  sn[30];
	uint8_t  mac_tag[3];
	uint8_t  mac_size;
	uint8_t  mac_addr[6];
};

static void rc4(unsigned char * buf, int len)
{
	unsigned char S[256], K[256], temp;
	unsigned char key[16] = { 124, 78, 3, 4, 85, 5, 9, 7, 45, 44, 123, 56, 23, 13, 23, 17 };
	int i, j, t, x;

	j = 0;
	for(i = 0; i < 256; i++)
	{
		S[i] = (unsigned char) i;
		j &= 0x0f;
		K[i] = key[j];
		j++;
	}

	j = 0;
	for(i = 0; i < 256; i++)
	{
		j = (j + S[i] + K[i]) % 256;
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
	}

	i = j = 0;
	for(x = 0; x < len; x++)
	{
		i = (i + 1) % 256;
		j = (j + S[i]) % 256;
		temp = S[i];
		S[i] = S[j];
		S[j] = temp;
		t = (S[i] + (S[j] % 256)) % 256;
		buf[x] = buf[x] ^ S[t];
	}
}

int main(int argc, char *argv[])
{
	FILE * fp;
	char * buffer;
	int buflen = 128 * 1024 * 1024;
	int llen, blen;
	int nbytes, reallen;
	struct idblock0_t * idb0;
	struct idblock1_t * idb1;
	struct idblock2_t * idb2;
	struct idblock3_t * idb3;

	if(argc != 4)
	{
		printf("Usage: mk3128 <destination> <loader> <bootloader>\n");
		return -1;
	}

	buffer = malloc(buflen);
	memset(buffer, 0, buflen);

	/* loader */
	fp = fopen(argv[2], "r+b");
	if(fp == NULL)
	{
		printf("Open file loader error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	llen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[LOADER_BLOCK_POSITION * BLKSIZE], 1, llen, fp);
	if(nbytes != llen)
	{
		printf("Read file loader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* bootloader */
	fp = fopen(argv[3], "r+b");
	if(fp == NULL)
	{
		printf("Open file bootloader error\n");
		free(buffer);
		return -1;
	}

	fseek(fp, 0L, SEEK_END);
	blen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	nbytes = fread(&buffer[BOOTLOADER_BLOCK_POSITION * BLKSIZE], 1, blen, fp);
	if(nbytes != blen)
	{
		printf("Read file bootloader error\n");
		free(buffer);
		fclose(fp);
		return -1;
	}
	fclose(fp);

	/* id block0 */
	idb0 = (struct idblock0_t *)(&buffer[(ID_BLOCK_POSITION + 0) * BLKSIZE]);
	idb0->sig = 0x0ff0aa55;
	idb0->rc4_flag = 1;
	idb0->bootcode1_offset = 0x04;
	idb0->bootcode2_offset = 0x04;
	idb0->flash_data_size = LOADER_BLOCK_SIZE;
	idb0->flash_boot_size = (blen + BLKSIZE) / BLKSIZE;
	rc4(&buffer[(ID_BLOCK_POSITION + 0) * BLKSIZE], BLKSIZE);

	/* id block1 */
	idb1 = (struct idblock1_t *)(&buffer[(ID_BLOCK_POSITION + 1) * BLKSIZE]);
	/* rc4(&buffer[(ID_BLOCK_POSITION + 1) * BLKSIZE], BLKSIZE); */

	/* id block2 */
	idb2 = (struct idblock2_t *)(&buffer[(ID_BLOCK_POSITION + 2) * BLKSIZE]);
	/* rc4(&buffer[(ID_BLOCK_POSITION + 2) * BLKSIZE], BLKSIZE); */

	/* id block3 */
	idb3 = (struct idblock3_t *)(&buffer[(ID_BLOCK_POSITION + 3) * BLKSIZE]);
	/* rc4(&buffer[(ID_BLOCK_POSITION + 3) * BLKSIZE], BLKSIZE); */

	/* destination */
	fp = fopen(argv[1], "w+b");
	if(fp == NULL)
	{
		printf("Destination file open error\n");
		free(buffer);
		return -1;
	}

	reallen = BOOTLOADER_BLOCK_POSITION * BLKSIZE + blen - (1 * BLKSIZE);
	nbytes = fwrite(&buffer[1 * BLKSIZE], 1, reallen, fp);
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
