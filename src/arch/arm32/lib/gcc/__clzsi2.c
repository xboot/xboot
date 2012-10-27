/*
 * __clzsi2.c for returns the leading number of 0 bits in the argument.
 */

/*
 * returns the leading number of 0 bits in the argument (32-bit).
 */
unsigned int __clzsi2(unsigned int v)
{
	signed int p = 31;

	if (v & 0xffff0000)
	{
		p -= 16;
		v >>= 16;
	}
	if (v & 0xff00)
	{
		p -= 8;
		v >>= 8;
	}
	if (v & 0xf0)
	{
		p -= 4;
		v >>= 4;
	}
	if (v & 0xc)
	{
		p -= 2;
		v >>= 2;
	}
	if (v & 0x2)
	{
		p -= 1;
		v >>= 1;
	}

	return p;
}
