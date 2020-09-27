/*
 * libx/qrcgen.c
 */

#include <qrcgen.h>

enum qrcgen_mode_t {
	QRCGEN_MODE_NUMERIC			= 0x1,
	QRCGEN_MODE_ALPHANUMERIC	= 0x2,
	QRCGEN_MODE_BYTE			= 0x4,
	QRCGEN_MODE_KANJI			= 0x8,
	QRCGEN_MODE_ECI				= 0x7,
};

struct qrcgen_segment_t {
	enum qrcgen_mode_t mode;
	int nchar;
	uint8_t * data;
	int blen;
};

static const char ALPHANUMERIC_CHARSET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

static const int8_t ECC_CODEWORDS_PER_BLOCK[4][41] = {
	{-1,  7, 10, 15, 20, 26, 18, 20, 24, 30, 18, 20, 24, 26, 30, 22, 24, 28, 30, 28, 28, 28, 28, 30, 30, 26, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 10, 16, 26, 18, 24, 16, 18, 22, 22, 26, 30, 22, 22, 24, 24, 28, 28, 26, 26, 26, 26, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28},
	{-1, 13, 22, 18, 26, 18, 24, 18, 22, 20, 24, 28, 26, 24, 20, 30, 24, 28, 28, 26, 30, 28, 30, 30, 30, 30, 28, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
	{-1, 17, 28, 22, 16, 22, 28, 26, 26, 24, 28, 24, 28, 22, 24, 24, 30, 28, 28, 26, 28, 30, 24, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30},
};

static const int8_t NUM_ERROR_CORRECTION_BLOCKS[4][41] = {
	{-1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 4,  4,  4,  4,  4,  6,  6,  6,  6,  7,  8,  8,  9,  9, 10, 12, 12, 12, 13, 14, 15, 16, 17, 18, 19, 19, 20, 21, 22, 24, 25},
	{-1, 1, 1, 1, 2, 2, 4, 4, 4, 5, 5,  5,  8,  9,  9, 10, 10, 11, 13, 14, 16, 17, 17, 18, 20, 21, 23, 25, 26, 28, 29, 31, 33, 35, 37, 38, 40, 43, 45, 47, 49},
	{-1, 1, 1, 2, 2, 4, 4, 6, 6, 8, 8,  8, 10, 12, 16, 12, 17, 16, 18, 21, 20, 23, 23, 25, 27, 29, 34, 34, 35, 38, 40, 43, 45, 48, 51, 53, 56, 59, 62, 65, 68},
	{-1, 1, 1, 2, 4, 4, 4, 5, 6, 8, 8, 11, 11, 16, 16, 18, 16, 19, 21, 25, 25, 25, 34, 30, 32, 35, 37, 40, 42, 45, 48, 51, 54, 57, 60, 63, 66, 70, 74, 77, 81},
};

static int numchar_count_bits(enum qrcgen_mode_t mode, int version)
{
	int i = (version + 7) / 17;

	switch(mode)
	{
	case QRCGEN_MODE_NUMERIC:
		return ((int[]){ 10, 12, 14 })[i];
	case QRCGEN_MODE_ALPHANUMERIC:
		return ((int[]){ 9, 11, 13 })[i];
	case QRCGEN_MODE_BYTE:
		return ((int[]){ 8, 16, 16 })[i];
	case QRCGEN_MODE_KANJI:
		return ((int[]){ 8, 10, 12 })[i];
	case QRCGEN_MODE_ECI:
		return 0;
	default:
		break;
	}
	return -1;
}

static int get_total_bits(struct qrcgen_segment_t * segs, int len, int version)
{
	int result = 0;
	int n, l, c;
	int i;

	for(i = 0; i < len; i++)
	{
		n = segs[i].nchar;
		l = segs[i].blen;
		c = numchar_count_bits(segs[i].mode, version);
		if(n >= (1 << c))
			return -1;
		result += 4 + c + l;
	}
	return result;
}

static void append_bits_to_buffer(unsigned int val, int nbits, uint8_t * buffer, int * bitlen)
{
	int i;

	for(i = nbits - 1; i >= 0; i--, (*bitlen)++)
		buffer[*bitlen >> 3] |= (((val >> i) & 1) << (7 - (*bitlen & 7)));
}

static int get_num_raw_data_modules(int version)
{
	int result = (16 * version + 128) * version + 64;
	int align;

	if(version >= 2)
	{
		align = version / 7 + 2;
		result -= (25 * align - 10) * align - 55;
		if(version >= 7)
			result -= 36;
	}
	return result;
}

static int get_bit(int x, int i)
{
	return ((x >> i) & 1) != 0;
}

static int get_module(const uint8_t * qrc, int x, int y)
{
	int qrs = qrc[0];
	int idx = y * qrs + x;
	return get_bit(qrc[(idx >> 3) + 1], idx & 7);
}

static void set_module(uint8_t * qrc, int x, int y, int isblack)
{
	int qrs = qrc[0];
	int idx = y * qrs + x;
	int bitidx = idx & 7;
	int byteidx = (idx >> 3) + 1;
	if(isblack)
		qrc[byteidx] |= 1 << bitidx;
	else
		qrc[byteidx] &= (1 << bitidx) ^ 0xff;
}

static void set_module_bounded(uint8_t * qrc, int x, int y, int isblack)
{
	int qrs = qrc[0];
	if(0 <= x && x < qrs && 0 <= y && y < qrs)
		set_module(qrc, x, y, isblack);
}

static int calc_segment_bit_length(enum qrcgen_mode_t mode, int nchar)
{
	int result = nchar;

	if(nchar > 0x7fff)
		return -1;
	if(mode == QRCGEN_MODE_NUMERIC)
		result = (result * 10 + 2) / 3;
	else if(mode == QRCGEN_MODE_ALPHANUMERIC)
		result = (result * 11 + 1) / 2;
	else if(mode == QRCGEN_MODE_BYTE)
		result *= 8;
	else if(mode == QRCGEN_MODE_KANJI)
		result *= 13;
	else if((mode == QRCGEN_MODE_ECI) && (nchar == 0))
		result = 3 * 8;
	else
		return -1;
	if(result > 0x7fff)
		return -1;
	return result;
}

static int get_num_data_code_words(int version, enum qrcgen_ecc_t ecc)
{
	return get_num_raw_data_modules(version) / 8 - ECC_CODEWORDS_PER_BLOCK[ecc][version] * NUM_ERROR_CORRECTION_BLOCKS[ecc][version];
}

static uint8_t reed_solomon_multiply(uint8_t x, uint8_t y)
{
	uint8_t z = 0;
	int i;

	for(i = 7; i >= 0; i--)
	{
		z = (uint8_t)((z << 1) ^ ((z >> 7) * 0x11d));
		z ^= ((y >> i) & 1) * x;
	}
	return z;
}

static void reed_solomon_compute_divisor(int degree, uint8_t * result)
{
	uint8_t root = 1;
	int i, j;

	memset(result, 0, degree * sizeof(result[0]));
	result[degree - 1] = 1;
	for(i = 0; i < degree; i++)
	{
		for(j = 0; j < degree; j++)
		{
			result[j] = reed_solomon_multiply(result[j], root);
			if(j + 1 < degree)
				result[j] ^= result[j + 1];
		}
		root = reed_solomon_multiply(root, 0x02);
	}
}

static void reed_solomon_compute_remainder(const uint8_t * data, int dlen, const uint8_t * generator, int degree, uint8_t * result)
{
	uint8_t factor;
	int i, j;

	memset(result, 0, degree * sizeof(result[0]));
	for(i = 0; i < dlen; i++)
	{
		factor = data[i] ^ result[0];
		memmove(&result[0], &result[1], (size_t)(degree - 1) * sizeof(result[0]));
		result[degree - 1] = 0;
		for(j = 0; j < degree; j++)
			result[j] ^= reed_solomon_multiply(generator[j], factor);
	}
}

static void add_ecc_and_interleave(uint8_t * data, int version, enum qrcgen_ecc_t ecc, uint8_t * result)
{
	int nblks = NUM_ERROR_CORRECTION_BLOCKS[ecc][version];
	int blkelen = ECC_CODEWORDS_PER_BLOCK[ecc][version];
	int raw = get_num_raw_data_modules(version) / 8;
	int dlen = get_num_data_code_words(version, ecc);
	int nsblks = nblks - raw % nblks;
	int sblkdlen = raw / nblks - blkelen;
	const uint8_t * dat = data;
	uint8_t rsdiv[30];
	uint8_t * e;
	int i, j, k, l, n;

	reed_solomon_compute_divisor(blkelen, rsdiv);
	for(i = 0; i < nblks; i++)
	{
		n = sblkdlen + (i < nsblks ? 0 : 1);
		e = &data[dlen];
		reed_solomon_compute_remainder(dat, n, rsdiv, blkelen, e);
		for(j = 0, k = i; j < n; j++, k += nblks)
		{
			if(j == sblkdlen)
				k -= nsblks;
			result[k] = dat[j];
		}
		for(l = 0, k = dlen + i; l < blkelen; l++, k += nblks)
			result[k] = e[l];
		dat += n;
	}
}

static void fill_rectangle(int left, int top, int width, int height, uint8_t * qrc)
{
	int dx, dy;

	for(dy = 0; dy < height; dy++)
	{
		for(dx = 0; dx < width; dx++)
			set_module(qrc, left + dx, top + dy, 1);
	}
}

static int get_alignment_pattern_position(int version, uint8_t * result)
{
	int align, step;
	int i, j;

	if(version == 1)
		return 0;
	align = version / 7 + 2;
	step = (version == 32) ? 26 : (version * 4 + align * 2 + 1) / (align * 2 - 2) * 2;
	for(i = align - 1, j = version * 4 + 10; i >= 1; i--, j -= step)
		result[i] = (uint8_t)j;
	result[0] = 6;
	return align;
}

static void initialize_function_modules(int version, uint8_t * qrc)
{
	uint8_t pos[7];
	int qrs = version * 4 + 17;
	int align;
	int i, j;

	memset(qrc, 0, ((qrs * qrs + 7) / 8 + 1) * sizeof(qrc[0]));
	qrc[0] = (uint8_t)qrs;

	fill_rectangle(6, 0, 1, qrs, qrc);
	fill_rectangle(0, 6, qrs, 1, qrc);
	fill_rectangle(0, 0, 9, 9, qrc);
	fill_rectangle(qrs - 8, 0, 8, 9, qrc);
	fill_rectangle(0, qrs - 8, 9, 8, qrc);

	align = get_alignment_pattern_position(version, pos);
	for(i = 0; i < align; i++)
	{
		for(j = 0; j < align; j++)
		{
			if(!((i == 0 && j == 0) || (i == 0 && j == align - 1) || (i == align - 1 && j == 0)))
				fill_rectangle(pos[i] - 2, pos[j] - 2, 5, 5, qrc);
		}
	}
	if(version >= 7)
	{
		fill_rectangle(qrs - 11, 0, 3, 6, qrc);
		fill_rectangle(0, qrs - 11, 6, 3, qrc);
	}
}

static void draw_code_words(const uint8_t * data, int dlen, uint8_t * qrc)
{
	int qrs = qrc[0];
	int right, vert;
	int x, y;
	int i = 0, j;

	for(right = qrs - 1; right >= 1; right -= 2)
	{
		if(right == 6)
			right = 5;
		for(vert = 0; vert < qrs; vert++)
		{
			for(j = 0; j < 2; j++)
			{
				x = right - j;
				y = (((right + 1) & 2) == 0) ? qrs - 1 - vert : vert;
				if(!get_module(qrc, x, y) && i < dlen * 8)
				{
					set_module(qrc, x, y, get_bit(data[i >> 3], 7 - (i & 7)));
					i++;
				}
			}
		}
	}
}

static void draw_white_function_modules(uint8_t * qrc, int version)
{
	uint8_t pos[7];
	long bits;
	int qrs = qrc[0];
	int align, rem;
	int dx, dy;
	int i, j, k;

	for(i = 7; i < qrs - 7; i += 2)
	{
		set_module(qrc, 6, i, 0);
		set_module(qrc, i, 6, 0);
	}
	for(dy = -4; dy <= 4; dy++)
	{
		for(dx = -4; dx <= 4; dx++)
		{
			int dist = abs(dx);
			if(abs(dy) > dist)
				dist = abs(dy);
			if(dist == 2 || dist == 4)
			{
				set_module_bounded(qrc, 3 + dx, 3 + dy, 0);
				set_module_bounded(qrc, qrs - 4 + dx, 3 + dy, 0);
				set_module_bounded(qrc, 3 + dx, qrs - 4 + dy, 0);
			}
		}
	}
	align = get_alignment_pattern_position(version, pos);
	for(i = 0; i < align; i++)
	{
		for(j = 0; j < align; j++)
		{
			if((i == 0 && j == 0) || (i == 0 && j == align - 1) || (i == align - 1 && j == 0))
				continue;
			for(dy = -1; dy <= 1; dy++)
			{
				for(dx = -1; dx <= 1; dx++)
					set_module(qrc, pos[i] + dx, pos[j] + dy, dx == 0 && dy == 0);
			}
		}
	}
	if(version >= 7)
	{
		rem = version;
		for(i = 0; i < 12; i++)
			rem = (rem << 1) ^ ((rem >> 11) * 0x1f25);
		bits = (version << 12) | rem;
		for(i = 0; i < 6; i++)
		{
			for(j = 0; j < 3; j++)
			{
				k = qrs - 11 + j;
				set_module(qrc, k, i, (bits & 1) != 0);
				set_module(qrc, i, k, (bits & 1) != 0);
				bits >>= 1;
			}
		}
	}
}

static void apply_mask(const uint8_t * m, uint8_t * qrc, enum qrcgen_mask_t mask)
{
	int qrs = qrc[0];
	int invert;
	int x, y;

	for(y = 0; y < qrs; y++)
	{
		for(x = 0; x < qrs; x++)
		{
			if(get_module(m, x, y))
				continue;
			switch(mask)
			{
			case QRCGEN_MASK_0:
				invert = (x + y) % 2 == 0;
				break;
			case QRCGEN_MASK_1:
				invert = y % 2 == 0;
				break;
			case QRCGEN_MASK_2:
				invert = x % 3 == 0;
				break;
			case QRCGEN_MASK_3:
				invert = (x + y) % 3 == 0;
				break;
			case QRCGEN_MASK_4:
				invert = (x / 3 + y / 2) % 2 == 0;
				break;
			case QRCGEN_MASK_5:
				invert = x * y % 2 + x * y % 3 == 0;
				break;
			case QRCGEN_MASK_6:
				invert = (x * y % 2 + x * y % 3) % 2 == 0;
				break;
			case QRCGEN_MASK_7:
				invert = ((x + y) % 2 + x * y % 3) % 2 == 0;
				break;
			default:
				return;
			}
			set_module(qrc, x, y, get_module(qrc, x, y) ^ invert);
		}
	}
}

static void draw_format_bits(enum qrcgen_ecc_t ecc, enum qrcgen_mask_t mask, uint8_t * qrc)
{
	int data = ((int[]){ 1, 0, 3, 2 })[ecc] << 3 | mask;
	int rem = data;
	int qrs, bits;
	int i;

	for(i = 0; i < 10; i++)
		rem = (rem << 1) ^ ((rem >> 9) * 0x537);
	bits = (data << 10 | rem) ^ 0x5412;
	for(i = 0; i <= 5; i++)
		set_module(qrc, 8, i, get_bit(bits, i));
	set_module(qrc, 8, 7, get_bit(bits, 6));
	set_module(qrc, 8, 8, get_bit(bits, 7));
	set_module(qrc, 7, 8, get_bit(bits, 8));
	for(i = 9; i < 15; i++)
		set_module(qrc, 14 - i, 8, get_bit(bits, i));
	qrs = qrc[0];
	for(i = 0; i < 8; i++)
		set_module(qrc, qrs - 1 - i, 8, get_bit(bits, i));
	for(i = 8; i < 15; i++)
		set_module(qrc, 8, qrs - 15 + i, get_bit(bits, i));
	set_module(qrc, 8, qrs - 8, 1);
}

static void finder_penalty_add_history(int crl, int * h, int qrs)
{
	if(h[0] == 0)
		crl += qrs;
	memmove(&h[1], &h[0], 6 * sizeof(h[0]));
	h[0] = crl;
}

static int finder_penalty_count_patterns(const int * h, int qrs)
{
	int n = h[1];
	int core = n > 0 && h[2] == n && h[3] == n * 3 && h[4] == n && h[5] == n;
	return (core && h[0] >= n * 4 && h[6] >= n ? 1 : 0) + (core && h[6] >= n * 4 && h[0] >= n ? 1 : 0);
}

static int finder_penalty_terminate_and_count(int col, int crl, int * h, int qrs)
{
	if(col)
	{
		finder_penalty_add_history(crl, h, qrs);
		crl = 0;
	}
	crl += qrs;
	finder_penalty_add_history(crl, h, qrs);
	return finder_penalty_count_patterns(h, qrs);
}

static long get_penalty_score(const uint8_t * qrc)
{
	int qrs = qrc[0];
	int total = qrs * qrs;
	int result = 0;
	int black = 0;
	int h[7], runx, runy, col;
	int x, y;

	for(y = 0; y < qrs; y++)
	{
		col = 0;
		runx = 0;
		memset(h, 0, sizeof(h));
		for(x = 0; x < qrs; x++)
		{
			if(get_module(qrc, x, y) == col)
			{
				runx++;
				if(runx == 5)
					result += 3;
				else if(runx > 5)
					result++;
			}
			else
			{
				finder_penalty_add_history(runx, h, qrs);
				if(!col)
					result += finder_penalty_count_patterns(h, qrs) * 40;
				col = get_module(qrc, x, y);
				runx = 1;
			}
		}
		result += finder_penalty_terminate_and_count(col, runx, h, qrs) * 40;
	}
	for(x = 0; x < qrs; x++)
	{
		col = 0;
		runy = 0;
		memset(h, 0, sizeof(h));
		for(y = 0; y < qrs; y++)
		{
			if(get_module(qrc, x, y) == col)
			{
				runy++;
				if(runy == 5)
					result += 3;
				else if(runy > 5)
					result++;
			}
			else
			{
				finder_penalty_add_history(runy, h, qrs);
				if(!col)
					result += finder_penalty_count_patterns(h, qrs) * 40;
				col = get_module(qrc, x, y);
				runy = 1;
			}
		}
		result += finder_penalty_terminate_and_count(col, runy, h, qrs) * 40;
	}
	for(y = 0; y < qrs - 1; y++)
	{
		for(x = 0; x < qrs - 1; x++)
		{
			col = get_module(qrc, x, y);
			if(col == get_module(qrc, x + 1, y) && col == get_module(qrc, x, y + 1) && col == get_module(qrc, x + 1, y + 1))
				result += 3;
		}
	}
	for(y = 0; y < qrs; y++)
	{
		for(x = 0; x < qrs; x++)
		{
			if(get_module(qrc, x, y))
				black++;
		}
	}
	result += ((abs(black * 20 - total * 10) + total - 1) / total - 1) * 10;
	return result;
}

static int qrcgen_is_numeric(const char * txt)
{
	for(; *txt != '\0'; txt++)
	{
		if(*txt < '0' || *txt > '9')
			return 0;
	}
	return 1;
}

static int qrcgen_is_alphanumeric(const char * txt)
{
	for(; *txt != '\0'; txt++)
	{
		if(strchr(ALPHANUMERIC_CHARSET, *txt) == NULL)
			return 0;
	}
	return 1;
}

static struct qrcgen_segment_t qrcgen_make_numeric(const char * digits, uint8_t * buf)
{
	struct qrcgen_segment_t result;
	size_t len = strlen(digits);
	result.mode = QRCGEN_MODE_NUMERIC;
	int blen = calc_segment_bit_length(result.mode, len);
	int adata = 0;
	int acnt = 0;
	char c;

	result.nchar = len;
	if(blen > 0)
		memset(buf, 0, ((size_t)blen + 7) / 8 * sizeof(buf[0]));
	result.blen = 0;
	for(; *digits != '\0'; digits++)
	{
		c = *digits;
		adata = adata * 10 + (c - '0');
		acnt++;
		if(acnt == 3)
		{
			append_bits_to_buffer(adata, 10, buf, &result.blen);
			adata = 0;
			acnt = 0;
		}
	}
	if(acnt > 0)
		append_bits_to_buffer(adata, acnt * 3 + 1, buf, &result.blen);
	result.data = buf;
	return result;
}

static struct qrcgen_segment_t qrcgen_make_alphanumeric(const char * txt, uint8_t * buf)
{
	struct qrcgen_segment_t result;
	size_t len = strlen(txt);
	result.mode = QRCGEN_MODE_ALPHANUMERIC;
	int blen = calc_segment_bit_length(result.mode, len);
	result.nchar = len;
	int adata = 0;
	int acnt = 0;
	char * tmp;

	if(blen > 0)
		memset(buf, 0, ((size_t)blen + 7) / 8 * sizeof(buf[0]));
	result.blen = 0;
	for(; *txt != '\0'; txt++)
	{
		tmp = strchr(ALPHANUMERIC_CHARSET, *txt);
		adata = adata * 45 + (tmp - ALPHANUMERIC_CHARSET);
		acnt++;
		if(acnt == 2)
		{
			append_bits_to_buffer(adata, 11, buf, &result.blen);
			adata = 0;
			acnt = 0;
		}
	}
	if(acnt > 0)
		append_bits_to_buffer(adata, 6, buf, &result.blen);
	result.data = buf;
	return result;
}

static size_t qrcgen_calc_segment_buffer_size(enum qrcgen_mode_t mode, size_t nchar)
{
	int temp = calc_segment_bit_length(mode, nchar);
	if(temp == -1)
		return 0x7fffffff;
	return (temp + 7) / 8;
}

static int qrcgen_encode_segments_advanced(struct qrcgen_segment_t * segs, size_t len, enum qrcgen_ecc_t ecc, int minv, int maxv, enum qrcgen_mask_t mask, int boost, uint8_t * tmp, uint8_t * qrc)
{
	struct qrcgen_segment_t * seg;
	long penalty, minpenalty;
	int blen = 0;
	int version, bits;
	int dcbits, tbits;
	int i, j;
	uint8_t pad;

	for(version = minv;; version++)
	{
		bits = get_total_bits(segs, len, version);
		if(bits != -1 && bits <= get_num_data_code_words(version, ecc) * 8)
			break;
		if(version >= maxv)
		{
			qrc[0] = 0;
			return 0;
		}
	}
	for(i = QRCGEN_ECC_MEDIUM; i <= QRCGEN_ECC_HIGH; i++)
	{
		if(boost && bits <= get_num_data_code_words(version, (enum qrcgen_ecc_t)i) * 8)
			ecc = (enum qrcgen_ecc_t)i;
	}
	memset(qrc, 0, (size_t)QRCGEN_BUFFER_LEN(version) * sizeof(qrc[0]));
	for(i = 0; i < len; i++)
	{
		seg = &segs[i];
		append_bits_to_buffer((unsigned int)seg->mode, 4, qrc, &blen);
		append_bits_to_buffer((unsigned int)seg->nchar, numchar_count_bits(seg->mode, version), qrc, &blen);
		for(j = 0; j < seg->blen; j++)
		{
			append_bits_to_buffer((unsigned int)((seg->data[j >> 3] >> (7 - (j & 7))) & 1), 1, qrc, &blen);
		}
	}

	dcbits = get_num_data_code_words(version, ecc) * 8;
	tbits = dcbits - blen;
	if(tbits > 4)
		tbits = 4;
	append_bits_to_buffer(0, tbits, qrc, &blen);
	append_bits_to_buffer(0, (8 - blen % 8) % 8, qrc, &blen);
	for(pad = 0xec; blen < dcbits; pad ^= 0xec ^ 0x11)
		append_bits_to_buffer(pad, 8, qrc, &blen);

	add_ecc_and_interleave(qrc, version, ecc, tmp);
	initialize_function_modules(version, qrc);
	draw_code_words(tmp, get_num_raw_data_modules(version) / 8, qrc);
	draw_white_function_modules(qrc, version);
	initialize_function_modules(version, tmp);

	if(mask == QRCGEN_MASK_AUTO)
	{
		minpenalty = 0x7fffffffffffffffL;
		for(i = 0; i < 8; i++)
		{
			apply_mask(tmp, qrc, i);
			draw_format_bits(ecc, i, qrc);
			penalty = get_penalty_score(qrc);
			if(penalty < minpenalty)
			{
				mask = i;
				minpenalty = penalty;
			}
			apply_mask(tmp, qrc, i);
		}
	}
	apply_mask(tmp, qrc, mask);
	draw_format_bits(ecc, mask, qrc);

	return 1;
}

int qrcgen_encode_text(const char * txt, uint8_t * tmp, uint8_t * qrc, enum qrcgen_ecc_t ecc, int minv, int maxv, enum qrcgen_mask_t mask, int boost)
{
	struct qrcgen_segment_t seg;
	int tlen, blen;
	int i;

	if(txt)
	{
		tlen = strlen(txt);
		if(tlen == 0)
			return qrcgen_encode_segments_advanced(NULL, 0, ecc, minv, maxv, mask, boost, tmp, qrc);
		blen = QRCGEN_BUFFER_LEN(maxv);

		if(qrcgen_is_numeric(txt))
		{
			if(qrcgen_calc_segment_buffer_size(QRCGEN_MODE_NUMERIC, tlen) > blen)
			{
				qrc[0] = 0;
				return 0;
			}
			seg = qrcgen_make_numeric(txt, tmp);
		}
		else if(qrcgen_is_alphanumeric(txt))
		{
			if(qrcgen_calc_segment_buffer_size(QRCGEN_MODE_ALPHANUMERIC, tlen) > blen)
			{
				qrc[0] = 0;
				return 0;
			}
			seg = qrcgen_make_alphanumeric(txt, tmp);
		}
		else
		{
			if(tlen > blen)
			{
				qrc[0] = 0;
				return 0;
			}
			for(i = 0; i < tlen; i++)
				tmp[i] = (uint8_t)txt[i];
			seg.mode = QRCGEN_MODE_BYTE;
			seg.blen = calc_segment_bit_length(seg.mode, tlen);
			if(seg.blen == -1)
			{
				qrc[0] = 0;
				return 0;
			}
			seg.nchar = (int)tlen;
			seg.data = tmp;
		}
		return qrcgen_encode_segments_advanced(&seg, 1, ecc, minv, maxv, mask, boost, tmp, qrc);
	}
	qrc[0] = 0;
	return 0;
}

int qrcgen_encode_binary(uint8_t * buf, int len, uint8_t * qrc, enum qrcgen_ecc_t ecc, int minv, int maxv, enum qrcgen_mask_t mask, int boost)
{
	struct qrcgen_segment_t seg;

	if(buf && (len > 0))
	{
		seg.mode = QRCGEN_MODE_BYTE;
		seg.blen = calc_segment_bit_length(seg.mode, len);
		if(seg.blen == -1)
		{
			qrc[0] = 0;
			return 0;
		}
		seg.nchar = len;
		seg.data = buf;
		return qrcgen_encode_segments_advanced(&seg, 1, ecc, minv, maxv, mask, boost, buf, qrc);
	}
	qrc[0] = 0;
	return 0;
}


int qrcgen_get_size(uint8_t * qrc)
{
	if(qrc)
		return qrc[0];
	return 0;
}

int qrcgen_get_pixel(uint8_t * qrc, int x, int y)
{
	if(qrc)
	{
		int qrs = qrc[0];
		if((0 <= x) && (x < qrs) && (0 <= y) && (y < qrs))
			return get_module(qrc, x, y);
	}
	return 0;
}
