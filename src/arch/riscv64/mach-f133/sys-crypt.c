/*
 * sys-crypt.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>

static void aes256_encrypt(uint8_t * xkey, uint8_t * sbox, uint8_t * x2sbox, uint8_t * x3sbox, uint8_t * in, uint8_t * out)
{
	uint8_t state[16];
	uint8_t tmp[16];
	uint8_t t;
	int i, j;

	memcpy(state, in, sizeof(state));
	for(j = 0; j < 16; j++)
		state[j] ^= xkey[j];
	for(i = 1; i < (14 + 1); i++)
	{
		if(i < 14)
		{
			tmp[0] = x2sbox[state[0]] ^ x3sbox[state[5]] ^ sbox[state[10]] ^ sbox[state[15]];
			tmp[1] = sbox[state[0]] ^ x2sbox[state[5]] ^ x3sbox[state[10]] ^ sbox[state[15]];
			tmp[2] = sbox[state[0]] ^ sbox[state[5]] ^ x2sbox[state[10]] ^ x3sbox[state[15]];
			tmp[3] = x3sbox[state[0]] ^ sbox[state[5]] ^ sbox[state[10]] ^ x2sbox[state[15]];
			tmp[4] = x2sbox[state[4]] ^ x3sbox[state[9]] ^ sbox[state[14]] ^ sbox[state[3]];
			tmp[5] = sbox[state[4]] ^ x2sbox[state[9]] ^ x3sbox[state[14]] ^ sbox[state[3]];
			tmp[6] = sbox[state[4]] ^ sbox[state[9]] ^ x2sbox[state[14]] ^ x3sbox[state[3]];
			tmp[7] = x3sbox[state[4]] ^ sbox[state[9]] ^ sbox[state[14]] ^ x2sbox[state[3]];
			tmp[8] = x2sbox[state[8]] ^ x3sbox[state[13]] ^ sbox[state[2]] ^ sbox[state[7]];
			tmp[9] = sbox[state[8]] ^ x2sbox[state[13]] ^ x3sbox[state[2]] ^ sbox[state[7]];
			tmp[10] = sbox[state[8]] ^ sbox[state[13]] ^ x2sbox[state[2]] ^ x3sbox[state[7]];
			tmp[11] = x3sbox[state[8]] ^ sbox[state[13]] ^ sbox[state[2]] ^ x2sbox[state[7]];
			tmp[12] = x2sbox[state[12]] ^ x3sbox[state[1]] ^ sbox[state[6]] ^ sbox[state[11]];
			tmp[13] = sbox[state[12]] ^ x2sbox[state[1]] ^ x3sbox[state[6]] ^ sbox[state[11]];
			tmp[14] = sbox[state[12]] ^ sbox[state[1]] ^ x2sbox[state[6]] ^ x3sbox[state[11]];
			tmp[15] = x3sbox[state[12]] ^ sbox[state[1]] ^ sbox[state[6]] ^ x2sbox[state[11]];
			memcpy(state, tmp, sizeof(tmp));
		}
		else
		{
			state[0] = sbox[state[0]];
			state[4] = sbox[state[4]];
			state[8] = sbox[state[8]];
			state[12] = sbox[state[12]];
			t = sbox[state[1]];
			state[1] = sbox[state[5]];
			state[5] = sbox[state[9]];
			state[9] = sbox[state[13]];
			state[13] = t;
			t = sbox[state[2]];
			state[2] = sbox[state[10]];
			state[10] = t;
			t = sbox[state[6]];
			state[6] = sbox[state[14]];
			state[14] = t;
			t = sbox[state[15]];
			state[15] = sbox[state[11]];
			state[11] = sbox[state[7]];
			state[7] = sbox[state[3]];
			state[3] = t;
		}
		for(j = 0; j < 16; j++)
			state[j] ^= xkey[(i << 4) + j];
	}
	memcpy(out, state, sizeof(state));
}

void sys_crypt(char * key, char * buf, int len)
{
	uint8_t sbox[256], x2sbox[256], x3sbox[256], rcon[11];
	uint8_t xkey[480];
	uint8_t counter[16];
	uint8_t tmp[16];
	uint8_t * p, t0, t1, t2, t3, t4;
	int l, i;

	p = sbox;
	p[  0] = 0x63; p[  1] = 0x7c; p[  2] = 0x77; p[  3] = 0x7b; p[  4] = 0xf2; p[  5] = 0x6b; p[  6] = 0x6f; p[  7] = 0xc5;
	p[  8] = 0x30; p[  9] = 0x01; p[ 10] = 0x67; p[ 11] = 0x2b; p[ 12] = 0xfe; p[ 13] = 0xd7; p[ 14] = 0xab; p[ 15] = 0x76;
	p[ 16] = 0xca; p[ 17] = 0x82; p[ 18] = 0xc9; p[ 19] = 0x7d; p[ 20] = 0xfa; p[ 21] = 0x59; p[ 22] = 0x47; p[ 23] = 0xf0;
	p[ 24] = 0xad; p[ 25] = 0xd4; p[ 26] = 0xa2; p[ 27] = 0xaf; p[ 28] = 0x9c; p[ 29] = 0xa4; p[ 30] = 0x72; p[ 31] = 0xc0;
	p[ 32] = 0xb7; p[ 33] = 0xfd; p[ 34] = 0x93; p[ 35] = 0x26; p[ 36] = 0x36; p[ 37] = 0x3f; p[ 38] = 0xf7; p[ 39] = 0xcc;
	p[ 40] = 0x34; p[ 41] = 0xa5; p[ 42] = 0xe5; p[ 43] = 0xf1; p[ 44] = 0x71; p[ 45] = 0xd8; p[ 46] = 0x31; p[ 47] = 0x15;
	p[ 48] = 0x04; p[ 49] = 0xc7; p[ 50] = 0x23; p[ 51] = 0xc3; p[ 52] = 0x18; p[ 53] = 0x96; p[ 54] = 0x05; p[ 55] = 0x9a;
	p[ 56] = 0x07; p[ 57] = 0x12; p[ 58] = 0x80; p[ 59] = 0xe2; p[ 60] = 0xeb; p[ 61] = 0x27; p[ 62] = 0xb2; p[ 63] = 0x75;
	p[ 64] = 0x09; p[ 65] = 0x83; p[ 66] = 0x2c; p[ 67] = 0x1a; p[ 68] = 0x1b; p[ 69] = 0x6e; p[ 70] = 0x5a; p[ 71] = 0xa0;
	p[ 72] = 0x52; p[ 73] = 0x3b; p[ 74] = 0xd6; p[ 75] = 0xb3; p[ 76] = 0x29; p[ 77] = 0xe3; p[ 78] = 0x2f; p[ 79] = 0x84;
	p[ 80] = 0x53; p[ 81] = 0xd1; p[ 82] = 0x00; p[ 83] = 0xed; p[ 84] = 0x20; p[ 85] = 0xfc; p[ 86] = 0xb1; p[ 87] = 0x5b;
	p[ 88] = 0x6a; p[ 89] = 0xcb; p[ 90] = 0xbe; p[ 91] = 0x39; p[ 92] = 0x4a; p[ 93] = 0x4c; p[ 94] = 0x58; p[ 95] = 0xcf;
	p[ 96] = 0xd0; p[ 97] = 0xef; p[ 98] = 0xaa; p[ 99] = 0xfb; p[100] = 0x43; p[101] = 0x4d; p[102] = 0x33; p[103] = 0x85;
	p[104] = 0x45; p[105] = 0xf9; p[106] = 0x02; p[107] = 0x7f; p[108] = 0x50; p[109] = 0x3c; p[110] = 0x9f; p[111] = 0xa8;
	p[112] = 0x51; p[113] = 0xa3; p[114] = 0x40; p[115] = 0x8f; p[116] = 0x92; p[117] = 0x9d; p[118] = 0x38; p[119] = 0xf5;
	p[120] = 0xbc; p[121] = 0xb6; p[122] = 0xda; p[123] = 0x21; p[124] = 0x10; p[125] = 0xff; p[126] = 0xf3; p[127] = 0xd2;
	p[128] = 0xcd; p[129] = 0x0c; p[130] = 0x13; p[131] = 0xec; p[132] = 0x5f; p[133] = 0x97; p[134] = 0x44; p[135] = 0x17;
	p[136] = 0xc4; p[137] = 0xa7; p[138] = 0x7e; p[139] = 0x3d; p[140] = 0x64; p[141] = 0x5d; p[142] = 0x19; p[143] = 0x73;
	p[144] = 0x60; p[145] = 0x81; p[146] = 0x4f; p[147] = 0xdc; p[148] = 0x22; p[149] = 0x2a; p[150] = 0x90; p[151] = 0x88;
	p[152] = 0x46; p[153] = 0xee; p[154] = 0xb8; p[155] = 0x14; p[156] = 0xde; p[157] = 0x5e; p[158] = 0x0b; p[159] = 0xdb;
	p[160] = 0xe0; p[161] = 0x32; p[162] = 0x3a; p[163] = 0x0a; p[164] = 0x49; p[165] = 0x06; p[166] = 0x24; p[167] = 0x5c;
	p[168] = 0xc2; p[169] = 0xd3; p[170] = 0xac; p[171] = 0x62; p[172] = 0x91; p[173] = 0x95; p[174] = 0xe4; p[175] = 0x79;
	p[176] = 0xe7; p[177] = 0xc8; p[178] = 0x37; p[179] = 0x6d; p[180] = 0x8d; p[181] = 0xd5; p[182] = 0x4e; p[183] = 0xa9;
	p[184] = 0x6c; p[185] = 0x56; p[186] = 0xf4; p[187] = 0xea; p[188] = 0x65; p[189] = 0x7a; p[190] = 0xae; p[191] = 0x08;
	p[192] = 0xba; p[193] = 0x78; p[194] = 0x25; p[195] = 0x2e; p[196] = 0x1c; p[197] = 0xa6; p[198] = 0xb4; p[199] = 0xc6;
	p[200] = 0xe8; p[201] = 0xdd; p[202] = 0x74; p[203] = 0x1f; p[204] = 0x4b; p[205] = 0xbd; p[206] = 0x8b; p[207] = 0x8a;
	p[208] = 0x70; p[209] = 0x3e; p[210] = 0xb5; p[211] = 0x66; p[212] = 0x48; p[213] = 0x03; p[214] = 0xf6; p[215] = 0x0e;
	p[216] = 0x61; p[217] = 0x35; p[218] = 0x57; p[219] = 0xb9; p[220] = 0x86; p[221] = 0xc1; p[222] = 0x1d; p[223] = 0x9e;
	p[224] = 0xe1; p[225] = 0xf8; p[226] = 0x98; p[227] = 0x11; p[228] = 0x69; p[229] = 0xd9; p[230] = 0x8e; p[231] = 0x94;
	p[232] = 0x9b; p[233] = 0x1e; p[234] = 0x87; p[235] = 0xe9; p[236] = 0xce; p[237] = 0x55; p[238] = 0x28; p[239] = 0xdf;
	p[240] = 0x8c; p[241] = 0xa1; p[242] = 0x89; p[243] = 0x0d; p[244] = 0xbf; p[245] = 0xe6; p[246] = 0x42; p[247] = 0x68;
	p[248] = 0x41; p[249] = 0x99; p[250] = 0x2d; p[251] = 0x0f; p[252] = 0xb0; p[253] = 0x54; p[254] = 0xbb; p[255] = 0x16;

	p = x2sbox;
	p[  0] = 0xc6; p[  1] = 0xf8; p[  2] = 0xee; p[  3] = 0xf6; p[  4] = 0xff; p[  5] = 0xd6; p[  6] = 0xde; p[  7] = 0x91;
	p[  8] = 0x60; p[  9] = 0x02; p[ 10] = 0xce; p[ 11] = 0x56; p[ 12] = 0xe7; p[ 13] = 0xb5; p[ 14] = 0x4d; p[ 15] = 0xec;
	p[ 16] = 0x8f; p[ 17] = 0x1f; p[ 18] = 0x89; p[ 19] = 0xfa; p[ 20] = 0xef; p[ 21] = 0xb2; p[ 22] = 0x8e; p[ 23] = 0xfb;
	p[ 24] = 0x41; p[ 25] = 0xb3; p[ 26] = 0x5f; p[ 27] = 0x45; p[ 28] = 0x23; p[ 29] = 0x53; p[ 30] = 0xe4; p[ 31] = 0x9b;
	p[ 32] = 0x75; p[ 33] = 0xe1; p[ 34] = 0x3d; p[ 35] = 0x4c; p[ 36] = 0x6c; p[ 37] = 0x7e; p[ 38] = 0xf5; p[ 39] = 0x83;
	p[ 40] = 0x68; p[ 41] = 0x51; p[ 42] = 0xd1; p[ 43] = 0xf9; p[ 44] = 0xe2; p[ 45] = 0xab; p[ 46] = 0x62; p[ 47] = 0x2a;
	p[ 48] = 0x08; p[ 49] = 0x95; p[ 50] = 0x46; p[ 51] = 0x9d; p[ 52] = 0x30; p[ 53] = 0x37; p[ 54] = 0x0a; p[ 55] = 0x2f;
	p[ 56] = 0x0e; p[ 57] = 0x24; p[ 58] = 0x1b; p[ 59] = 0xdf; p[ 60] = 0xcd; p[ 61] = 0x4e; p[ 62] = 0x7f; p[ 63] = 0xea;
	p[ 64] = 0x12; p[ 65] = 0x1d; p[ 66] = 0x58; p[ 67] = 0x34; p[ 68] = 0x36; p[ 69] = 0xdc; p[ 70] = 0xb4; p[ 71] = 0x5b;
	p[ 72] = 0xa4; p[ 73] = 0x76; p[ 74] = 0xb7; p[ 75] = 0x7d; p[ 76] = 0x52; p[ 77] = 0xdd; p[ 78] = 0x5e; p[ 79] = 0x13;
	p[ 80] = 0xa6; p[ 81] = 0xb9; p[ 82] = 0x00; p[ 83] = 0xc1; p[ 84] = 0x40; p[ 85] = 0xe3; p[ 86] = 0x79; p[ 87] = 0xb6;
	p[ 88] = 0xd4; p[ 89] = 0x8d; p[ 90] = 0x67; p[ 91] = 0x72; p[ 92] = 0x94; p[ 93] = 0x98; p[ 94] = 0xb0; p[ 95] = 0x85;
	p[ 96] = 0xbb; p[ 97] = 0xc5; p[ 98] = 0x4f; p[ 99] = 0xed; p[100] = 0x86; p[101] = 0x9a; p[102] = 0x66; p[103] = 0x11;
	p[104] = 0x8a; p[105] = 0xe9; p[106] = 0x04; p[107] = 0xfe; p[108] = 0xa0; p[109] = 0x78; p[110] = 0x25; p[111] = 0x4b;
	p[112] = 0xa2; p[113] = 0x5d; p[114] = 0x80; p[115] = 0x05; p[116] = 0x3f; p[117] = 0x21; p[118] = 0x70; p[119] = 0xf1;
	p[120] = 0x63; p[121] = 0x77; p[122] = 0xaf; p[123] = 0x42; p[124] = 0x20; p[125] = 0xe5; p[126] = 0xfd; p[127] = 0xbf;
	p[128] = 0x81; p[129] = 0x18; p[130] = 0x26; p[131] = 0xc3; p[132] = 0xbe; p[133] = 0x35; p[134] = 0x88; p[135] = 0x2e;
	p[136] = 0x93; p[137] = 0x55; p[138] = 0xfc; p[139] = 0x7a; p[140] = 0xc8; p[141] = 0xba; p[142] = 0x32; p[143] = 0xe6;
	p[144] = 0xc0; p[145] = 0x19; p[146] = 0x9e; p[147] = 0xa3; p[148] = 0x44; p[149] = 0x54; p[150] = 0x3b; p[151] = 0x0b;
	p[152] = 0x8c; p[153] = 0xc7; p[154] = 0x6b; p[155] = 0x28; p[156] = 0xa7; p[157] = 0xbc; p[158] = 0x16; p[159] = 0xad;
	p[160] = 0xdb; p[161] = 0x64; p[162] = 0x74; p[163] = 0x14; p[164] = 0x92; p[165] = 0x0c; p[166] = 0x48; p[167] = 0xb8;
	p[168] = 0x9f; p[169] = 0xbd; p[170] = 0x43; p[171] = 0xc4; p[172] = 0x39; p[173] = 0x31; p[174] = 0xd3; p[175] = 0xf2;
	p[176] = 0xd5; p[177] = 0x8b; p[178] = 0x6e; p[179] = 0xda; p[180] = 0x01; p[181] = 0xb1; p[182] = 0x9c; p[183] = 0x49;
	p[184] = 0xd8; p[185] = 0xac; p[186] = 0xf3; p[187] = 0xcf; p[188] = 0xca; p[189] = 0xf4; p[190] = 0x47; p[191] = 0x10;
	p[192] = 0x6f; p[193] = 0xf0; p[194] = 0x4a; p[195] = 0x5c; p[196] = 0x38; p[197] = 0x57; p[198] = 0x73; p[199] = 0x97;
	p[200] = 0xcb; p[201] = 0xa1; p[202] = 0xe8; p[203] = 0x3e; p[204] = 0x96; p[205] = 0x61; p[206] = 0x0d; p[207] = 0x0f;
	p[208] = 0xe0; p[209] = 0x7c; p[210] = 0x71; p[211] = 0xcc; p[212] = 0x90; p[213] = 0x06; p[214] = 0xf7; p[215] = 0x1c;
	p[216] = 0xc2; p[217] = 0x6a; p[218] = 0xae; p[219] = 0x69; p[220] = 0x17; p[221] = 0x99; p[222] = 0x3a; p[223] = 0x27;
	p[224] = 0xd9; p[225] = 0xeb; p[226] = 0x2b; p[227] = 0x22; p[228] = 0xd2; p[229] = 0xa9; p[230] = 0x07; p[231] = 0x33;
	p[232] = 0x2d; p[233] = 0x3c; p[234] = 0x15; p[235] = 0xc9; p[236] = 0x87; p[237] = 0xaa; p[238] = 0x50; p[239] = 0xa5;
	p[240] = 0x03; p[241] = 0x59; p[242] = 0x09; p[243] = 0x1a; p[244] = 0x65; p[245] = 0xd7; p[246] = 0x84; p[247] = 0xd0;
	p[248] = 0x82; p[249] = 0x29; p[250] = 0x5a; p[251] = 0x1e; p[252] = 0x7b; p[253] = 0xa8; p[254] = 0x6d; p[255] = 0x2c;

	p = x3sbox;
	p[  0] = 0xa5; p[  1] = 0x84; p[  2] = 0x99; p[  3] = 0x8d; p[  4] = 0x0d; p[  5] = 0xbd; p[  6] = 0xb1; p[  7] = 0x54;
	p[  8] = 0x50; p[  9] = 0x03; p[ 10] = 0xa9; p[ 11] = 0x7d; p[ 12] = 0x19; p[ 13] = 0x62; p[ 14] = 0xe6; p[ 15] = 0x9a;
	p[ 16] = 0x45; p[ 17] = 0x9d; p[ 18] = 0x40; p[ 19] = 0x87; p[ 20] = 0x15; p[ 21] = 0xeb; p[ 22] = 0xc9; p[ 23] = 0x0b;
	p[ 24] = 0xec; p[ 25] = 0x67; p[ 26] = 0xfd; p[ 27] = 0xea; p[ 28] = 0xbf; p[ 29] = 0xf7; p[ 30] = 0x96; p[ 31] = 0x5b;
	p[ 32] = 0xc2; p[ 33] = 0x1c; p[ 34] = 0xae; p[ 35] = 0x6a; p[ 36] = 0x5a; p[ 37] = 0x41; p[ 38] = 0x02; p[ 39] = 0x4f;
	p[ 40] = 0x5c; p[ 41] = 0xf4; p[ 42] = 0x34; p[ 43] = 0x08; p[ 44] = 0x93; p[ 45] = 0x73; p[ 46] = 0x53; p[ 47] = 0x3f;
	p[ 48] = 0x0c; p[ 49] = 0x52; p[ 50] = 0x65; p[ 51] = 0x5e; p[ 52] = 0x28; p[ 53] = 0xa1; p[ 54] = 0x0f; p[ 55] = 0xb5;
	p[ 56] = 0x09; p[ 57] = 0x36; p[ 58] = 0x9b; p[ 59] = 0x3d; p[ 60] = 0x26; p[ 61] = 0x69; p[ 62] = 0xcd; p[ 63] = 0x9f;
	p[ 64] = 0x1b; p[ 65] = 0x9e; p[ 66] = 0x74; p[ 67] = 0x2e; p[ 68] = 0x2d; p[ 69] = 0xb2; p[ 70] = 0xee; p[ 71] = 0xfb;
	p[ 72] = 0xf6; p[ 73] = 0x4d; p[ 74] = 0x61; p[ 75] = 0xce; p[ 76] = 0x7b; p[ 77] = 0x3e; p[ 78] = 0x71; p[ 79] = 0x97;
	p[ 80] = 0xf5; p[ 81] = 0x68; p[ 82] = 0x00; p[ 83] = 0x2c; p[ 84] = 0x60; p[ 85] = 0x1f; p[ 86] = 0xc8; p[ 87] = 0xed;
	p[ 88] = 0xbe; p[ 89] = 0x46; p[ 90] = 0xd9; p[ 91] = 0x4b; p[ 92] = 0xde; p[ 93] = 0xd4; p[ 94] = 0xe8; p[ 95] = 0x4a;
	p[ 96] = 0x6b; p[ 97] = 0x2a; p[ 98] = 0xe5; p[ 99] = 0x16; p[100] = 0xc5; p[101] = 0xd7; p[102] = 0x55; p[103] = 0x94;
	p[104] = 0xcf; p[105] = 0x10; p[106] = 0x06; p[107] = 0x81; p[108] = 0xf0; p[109] = 0x44; p[110] = 0xba; p[111] = 0xe3;
	p[112] = 0xf3; p[113] = 0xfe; p[114] = 0xc0; p[115] = 0x8a; p[116] = 0xad; p[117] = 0xbc; p[118] = 0x48; p[119] = 0x04;
	p[120] = 0xdf; p[121] = 0xc1; p[122] = 0x75; p[123] = 0x63; p[124] = 0x30; p[125] = 0x1a; p[126] = 0x0e; p[127] = 0x6d;
	p[128] = 0x4c; p[129] = 0x14; p[130] = 0x35; p[131] = 0x2f; p[132] = 0xe1; p[133] = 0xa2; p[134] = 0xcc; p[135] = 0x39;
	p[136] = 0x57; p[137] = 0xf2; p[138] = 0x82; p[139] = 0x47; p[140] = 0xac; p[141] = 0xe7; p[142] = 0x2b; p[143] = 0x95;
	p[144] = 0xa0; p[145] = 0x98; p[146] = 0xd1; p[147] = 0x7f; p[148] = 0x66; p[149] = 0x7e; p[150] = 0xab; p[151] = 0x83;
	p[152] = 0xca; p[153] = 0x29; p[154] = 0xd3; p[155] = 0x3c; p[156] = 0x79; p[157] = 0xe2; p[158] = 0x1d; p[159] = 0x76;
	p[160] = 0x3b; p[161] = 0x56; p[162] = 0x4e; p[163] = 0x1e; p[164] = 0xdb; p[165] = 0x0a; p[166] = 0x6c; p[167] = 0xe4;
	p[168] = 0x5d; p[169] = 0x6e; p[170] = 0xef; p[171] = 0xa6; p[172] = 0xa8; p[173] = 0xa4; p[174] = 0x37; p[175] = 0x8b;
	p[176] = 0x32; p[177] = 0x43; p[178] = 0x59; p[179] = 0xb7; p[180] = 0x8c; p[181] = 0x64; p[182] = 0xd2; p[183] = 0xe0;
	p[184] = 0xb4; p[185] = 0xfa; p[186] = 0x07; p[187] = 0x25; p[188] = 0xaf; p[189] = 0x8e; p[190] = 0xe9; p[191] = 0x18;
	p[192] = 0xd5; p[193] = 0x88; p[194] = 0x6f; p[195] = 0x72; p[196] = 0x24; p[197] = 0xf1; p[198] = 0xc7; p[199] = 0x51;
	p[200] = 0x23; p[201] = 0x7c; p[202] = 0x9c; p[203] = 0x21; p[204] = 0xdd; p[205] = 0xdc; p[206] = 0x86; p[207] = 0x85;
	p[208] = 0x90; p[209] = 0x42; p[210] = 0xc4; p[211] = 0xaa; p[212] = 0xd8; p[213] = 0x05; p[214] = 0x01; p[215] = 0x12;
	p[216] = 0xa3; p[217] = 0x5f; p[218] = 0xf9; p[219] = 0xd0; p[220] = 0x91; p[221] = 0x58; p[222] = 0x27; p[223] = 0xb9;
	p[224] = 0x38; p[225] = 0x13; p[226] = 0xb3; p[227] = 0x33; p[228] = 0xbb; p[229] = 0x70; p[230] = 0x89; p[231] = 0xa7;
	p[232] = 0xb6; p[233] = 0x22; p[234] = 0x92; p[235] = 0x20; p[236] = 0x49; p[237] = 0xff; p[238] = 0x78; p[239] = 0x7a;
	p[240] = 0x8f; p[241] = 0xf8; p[242] = 0x80; p[243] = 0x17; p[244] = 0xda; p[245] = 0x31; p[246] = 0xc6; p[247] = 0xb8;
	p[248] = 0xc3; p[249] = 0xb0; p[250] = 0x77; p[251] = 0x11; p[252] = 0xcb; p[253] = 0xfc; p[254] = 0xd6; p[255] = 0x3a;

	p = rcon;
	p[  0] = 0x00; p[  1] = 0x01; p[  2] = 0x02; p[  3] = 0x04; p[  4] = 0x08; p[  5] = 0x10; p[  6] = 0x20; p[  7] = 0x40;
	p[  8] = 0x80; p[  9] = 0x1b; p[ 10] = 0x36;

	memcpy(xkey, key, 32);
	for(i = 8; i < ((14 + 1) << 2); i++)
	{
		t0 = xkey[(i << 2) - 4];
		t1 = xkey[(i << 2) - 3];
		t2 = xkey[(i << 2) - 2];
		t3 = xkey[(i << 2) - 1];
		if(!(i & 0x7))
		{
			t4 = t3;
			t3 = sbox[t0];
			t0 = sbox[t1] ^ rcon[i >> 3];
			t1 = sbox[t2];
			t2 = sbox[t4];
		}
		else if((i & 0x7) == 0x4)
		{
			t0 = sbox[t0];
			t1 = sbox[t1];
			t2 = sbox[t2];
			t3 = sbox[t3];
		}
		xkey[(i << 2) + 0] = xkey[(i << 2) - 32 + 0] ^ t0;
		xkey[(i << 2) + 1] = xkey[(i << 2) - 32 + 1] ^ t1;
		xkey[(i << 2) + 2] = xkey[(i << 2) - 32 + 2] ^ t2;
		xkey[(i << 2) + 3] = xkey[(i << 2) - 32 + 3] ^ t3;
	}

	for(i = 0; i < sizeof(counter); i++)
		counter[i] = 0;
	while(len > 0)
	{
		l = len > 16 ? 16 : len;
		len -= l;
		aes256_encrypt(xkey, sbox, x2sbox, x3sbox, counter, tmp);
		for(i = 0; i < l; i++)
			buf[i] ^= tmp[i];
		buf += l;
		for(i = 15; i >= 8; i--)
		{
			counter[i]++;
			if((counter[i] != 0) || (i == 8))
				break;
		}
	}
}
