/*
 * __ashrdi3.c for 64-bit arithmetic shift right
 */

/*
 * 64-bit arithmetic shift right
 */
unsigned long long __ashrdi3(unsigned long long v, signed int cnt)
{
	signed int c = cnt & 31;
	unsigned int vl = (unsigned int) v;
	unsigned int vh = (unsigned int) (v >> 32);

	if (cnt & 32)
	{
		vl = ((signed int) vh >> c);
		vh = (signed int) vh >> 31;
	}
	else
	{
		vl = (vl >> c) + (vh << (32 - c));
		vh = ((signed int) vh >> c);
	}

	return ((unsigned long long) vh << 32) + vl;
}
