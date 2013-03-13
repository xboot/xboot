/*
 * drivers/mtd/nand/nand_ids.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <types.h>
#include <stddef.h>
#include <mtd/nand/nand_ids.h>

/*
 * name, id code, page size, chip size, erase size,	options
 *
 * page size:
 * 0:	get this information from the extended chip id
 * 256:	256 byte page size
 * 512:	512 byte page size
 */
struct nand_info nand_flash_ids[] = {
	{"NAND 16MiB 1.8V 8-bit",	0x33, 	512, 	16, 	0x4000, 0},
	{"NAND 16MiB 3.3V 8-bit",	0x73, 	512, 	16,		0x4000, 0},
	{"NAND 16MiB 1.8V 16-bit",	0x43, 	512, 	16, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 16MiB 3.3V 16-bit",	0x53, 	512, 	16, 	0x4000, NAND_BUSWIDTH_16},

	{"NAND 32MiB 1.8V 8-bit",	0x35, 	512, 	32, 	0x4000, 0},
	{"NAND 32MiB 3.3V 8-bit",	0x75, 	512, 	32, 	0x4000, 0},
	{"NAND 32MiB 1.8V 16-bit",	0x45, 	512, 	32, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 32MiB 3.3V 16-bit",	0x55, 	512, 	32, 	0x4000, NAND_BUSWIDTH_16},

	{"NAND 64MiB 1.8V 8-bit",	0x36, 	512, 	64, 	0x4000, 0},
	{"NAND 64MiB 3.3V 8-bit",	0x76, 	512, 	64, 	0x4000, 0},
	{"NAND 64MiB 1.8V 16-bit",	0x46, 	512, 	64, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 64MiB 3.3V 16-bit",	0x56,	512, 	64, 	0x4000, NAND_BUSWIDTH_16},

	{"NAND 128MiB 1.8V 8-bit",	0x78,	512, 	128, 	0x4000, 0},
	{"NAND 128MiB 1.8V 8-bit",	0x39,	512, 	128, 	0x4000, 0},
	{"NAND 128MiB 3.3V 8-bit",	0x79,	512, 	128, 	0x4000, 0},
	{"NAND 128MiB 1.8V 16-bit",	0x72, 	512, 	128, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 128MiB 1.8V 16-bit",	0x49,	512, 	128, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 128MiB 3.3V 16-bit",	0x74,	512, 	128, 	0x4000, NAND_BUSWIDTH_16},
	{"NAND 128MiB 3.3V 16-bit",	0x59,	512,	128, 	0x4000, NAND_BUSWIDTH_16},

	{"NAND 256MiB 3.3V 8-bit",	0x71,	512,	256,	0x4000, 0},

	/* 512 Megabit */
	{"NAND 64MiB 1.8V 8-bit",	0xA2,	0,		64,		0,		LP_OPTIONS},
	{"NAND 64MiB 3.3V 8-bit",	0xF2,	0,		64,		0, 		LP_OPTIONS},
	{"NAND 64MiB 1.8V 16-bit",	0xB2, 	0,		64, 	0, 		LP_OPTIONS16},
	{"NAND 64MiB 3.3V 16-bit",	0xC2, 	0,		64, 	0, 		LP_OPTIONS16},

	/* 1 Gigabit */
	{"NAND 128MiB 1.8V 8-bit",	0xA1, 	0, 		128, 	0, 		LP_OPTIONS},
	{"NAND 128MiB 3.3V 8-bit",	0xF1, 	0, 		128, 	0, 		LP_OPTIONS},
	{"NAND 128MiB 1.8V 16-bit",	0xB1, 	0, 		128, 	0, 		LP_OPTIONS16},
	{"NAND 128MiB 3.3V 16-bit",	0xC1, 	0, 		128, 	0, 		LP_OPTIONS16},

	/* 2 Gigabit */
	{"NAND 256MiB 1.8V 8-bit",	0xAA,	0, 		256, 	0, 		LP_OPTIONS},
	{"NAND 256MiB 3.3V 8-bit",	0xDA, 	0, 		256, 	0, 		LP_OPTIONS},
	{"NAND 256MiB 1.8V 16-bit",	0xBA, 	0, 		256, 	0, 		LP_OPTIONS16},
	{"NAND 256MiB 3.3V 16-bit",	0xCA,	0, 		256, 	0, 		LP_OPTIONS16},

	/* 4 Gigabit */
	{"NAND 512MiB 1.8V 8-bit",	0xAC, 	0, 		512, 	0, 		LP_OPTIONS},
	{"NAND 512MiB 3.3V 8-bit",	0xDC, 	0, 		512, 	0, 		LP_OPTIONS},
	{"NAND 512MiB 1.8V 16-bit",	0xBC, 	0, 		512, 	0, 		LP_OPTIONS16},
	{"NAND 512MiB 3.3V 16-bit",	0xCC, 	0, 		512, 	0, 		LP_OPTIONS16},

	/* 8 Gigabit */
	{"NAND 1GiB 1.8V 8-bit",	0xA3, 	0,		1024, 	0, 		LP_OPTIONS},
	{"NAND 1GiB 3.3V 8-bit",	0xD3, 	0, 		1024, 	0, 		LP_OPTIONS},
	{"NAND 1GiB 1.8V 16-bit",	0xB3, 	0, 		1024, 	0, 		LP_OPTIONS16},
	{"NAND 1GiB 3.3V 16-bit",	0xC3, 	0, 		1024, 	0, 		LP_OPTIONS16},

	/* 16 Gigabit */
	{"NAND 2GiB 1.8V 8-bit",	0xA5,	0, 		2048, 	0, 		LP_OPTIONS},
	{"NAND 2GiB 3.3V 8-bit",	0xD5,	0, 		2048, 	0, 		LP_OPTIONS},
	{"NAND 2GiB 1.8V 16-bit",	0xB5,	0,		2048, 	0, 		LP_OPTIONS16},
	{"NAND 2GiB 3.3V 16-bit",	0xC5,	0,		2048,	0,		LP_OPTIONS16},

	/* null */
	{NULL,						0,		0,		0,		0,		0}
};

/*
 * manufacturer id list
 */
struct nand_manufacturer nand_manuf_ids[] = {
	{NAND_MFR_UNKOWN,	"Unkown"},
	{NAND_MFR_TOSHIBA,	"Toshiba"},
	{NAND_MFR_SAMSUNG,	"Samsung"},
	{NAND_MFR_FUJITSU,	"Fujitsu"},
	{NAND_MFR_NATIONAL,	"National"},
	{NAND_MFR_RENESAS,	"Renesas"},
	{NAND_MFR_STMICRO,	"ST Micro"},
	{NAND_MFR_HYNIX,	"Hynix"},
	{NAND_MFR_MICRON,	"Micron"},
	{NAND_MFR_AMD,		"AMD"},
	{0x0,				NULL},
};
