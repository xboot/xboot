/*
 * __muldi3.c for 64-bit unsigned integer multiply.
 */

/*
 * 64-bit unsigned integer multiply.
 */
unsigned long long __muldi3(unsigned long long a, unsigned long long b)
{
	unsigned int al = (unsigned int)a;
	unsigned int ah = (unsigned int)(a>>32);
	unsigned int bl = (unsigned int)b;
	unsigned int bh = (unsigned int)(b>>32);
	unsigned long long v;

	v = (unsigned long long)al*bl;
	v += (unsigned long long)(al*bh + ah*bl)<<32;

	return v;
}
