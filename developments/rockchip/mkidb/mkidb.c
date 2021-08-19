#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BLKSIZE		(512)

struct idblock0_t {
	uint32_t signature;
	uint8_t	 reserved0[4];
	uint32_t disable_rc4;
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
	uint8_t  reserved0[72];
	uint16_t flash_data_offset;
	uint16_t flash_data_len;
	uint8_t  reserved1[384];
	uint32_t flash_chip_size;
	uint8_t  reserved2;
	uint8_t  access_time;
	uint16_t phy_block_size;
	uint8_t  phy_page_size;
	uint8_t  ecc_bits;

	uint8_t  reserved3[8];
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
	uint8_t  reserved0[468];
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
	struct idblock0_t * idb0;
	struct idblock1_t * idb1;
	struct idblock2_t * idb2;
	struct idblock3_t * idb3;
	FILE * fp;
	char * buf;
	int buflen = 256 * 1024 * 1024;
	int dlen, blen;
	int dblk, bblk;
	int n, l;

	if(argc != 4)
	{
		printf("Usage: mkidb <ddr> <boot> <bootpak>\n");
		return -1;
	}

	buf = malloc(buflen);
	if(!buf)
	{
		printf("Malloc buffer error\n");
		return -1;
	}
	memset(buf, 0, buflen);

	/* ddr */
	fp = fopen(argv[1], "r+b");
	if(fp == NULL)
	{
		printf("Open ddr file error\n");
		free(buf);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	dlen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	n = fread(&buf[4 * BLKSIZE], 1, dlen, fp);
	if(n != dlen)
	{
		printf("Read file boot error\n");
		free(buf);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	dblk = (dlen + BLKSIZE - 1) / BLKSIZE;
	dblk = (dblk + 3) & ~3;

	/* boot */
	fp = fopen(argv[2], "r+b");
	if(fp == NULL)
	{
		printf("Open boot file error\n");
		free(buf);
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	blen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	n = fread(&buf[(4 + dblk) * BLKSIZE], 1, blen, fp);
	if(n != blen)
	{
		printf("Read boot file error\n");
		free(buf);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	bblk = (blen + BLKSIZE - 1) / BLKSIZE;
	bblk = (bblk + 3) & ~3;

	/* idb0 */
	idb0 = (struct idblock0_t *)&buf[0 * BLKSIZE];
	idb0->signature = 0x0ff0aa55;
	idb0->disable_rc4 = 1;
	idb0->bootcode1_offset = 4;
	idb0->bootcode2_offset = 4;
	idb0->flash_data_size = dblk;
	idb0->flash_boot_size = dblk + bblk;
	rc4((unsigned char *)idb0, sizeof(struct idblock0_t));

	/* idb1 */
	idb1 = (struct idblock1_t *)&buf[1 * BLKSIZE];
	rc4((unsigned char *)idb1, sizeof(struct idblock1_t));

	/* idb2 */
	idb2 = (struct idblock2_t *)&buf[2 * BLKSIZE];
	rc4((unsigned char *)idb2, sizeof(struct idblock2_t));

	/* idb3 */
	idb3 = (struct idblock3_t *)&buf[3 * BLKSIZE];
	rc4((unsigned char *)idb3, sizeof(struct idblock3_t));

	/* idbloader */
	fp = fopen(argv[3], "w+b");
	if(fp == NULL)
	{
		printf("Open bootpak file error\n");
		free(buf);
		return -1;
	}
	l = (4 + dblk + bblk) * BLKSIZE;
	n = fwrite(buf, 1, l, fp);
	if(n != l)
	{
		printf("Write bootpak file error\n");
		free(buf);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	free(buf);

	printf("Binary pack file -> %s\n", argv[3]);
	return 0;
}
