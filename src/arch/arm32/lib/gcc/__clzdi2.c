/*
 * __clzdi2.c for returns the leading number of 0 bits in the argument.
 */

/*
 * returns the leading number of 0 bits in the argument (64-bit).
 */
unsigned int __clzdi2(unsigned long long v)
{
	signed int p = 63;

	if(v & 0xffffffff00000000ULL)
	{
		p -= 32;
		v >>= 32;
	}
	if(v & 0xffff0000UL)
	{
		p -= 16;
		v >>= 16;
	}
	if(v & 0xff00)
	{
		p -= 8;
		v >>= 8;
	}
	if(v & 0xf0)
	{
		p -= 4;
		v >>= 4;
	}
	if(v & 0xc)
	{
		p -= 2;
		v >>= 2;
	}
	if(v & 0x2)
	{
		p -= 1;
		v >>= 1;
	}

	return p;
}
