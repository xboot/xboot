/*
 * __lshrdi3.c for 64-bit logical shift right
 */

/*
 * 64-bit logical shift right
 */
unsigned long long __lshrdi3(unsigned long long v, signed int cnt)
{
	signed int c = cnt & 31;
	unsigned int vl = (unsigned int) v;
	unsigned int vh = (unsigned int) (v >> 32);

	if (cnt & 32)
	{
		vl = (vh >> c);
		vh = 0;
	}
	else
	{
		vl = (vl >> c) + (vh << (32 - c));
		vh = (vh >> c);
	}

	return ((unsigned long long) vh << 32) + vl;
}
