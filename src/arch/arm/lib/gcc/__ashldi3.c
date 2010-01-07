/*
 * __ashldi3.c for 64-bit arithmetic shift left
 */

/*
 * 64-bit arithmetic shift left
 */
unsigned long long __ashldi3(unsigned long long v, signed int cnt)
{
	signed int c = cnt & 31;
	unsigned int vl = (unsigned int) v;
	unsigned int vh = (unsigned int) (v >> 32);

	if (cnt & 32)
	{
		vh = (vl << c);
		vl = 0;
	}
	else
	{
		vh = (vh << c) + (vl >> (32 - c));
		vl = (vl << c);
	}

	return ((unsigned long long) vh << 32) + vl;
}
