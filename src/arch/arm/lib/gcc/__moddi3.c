/*
 * __moddi3.c for 64-bit signed integer modulo.
 */

extern unsigned long long __udivmoddi4(unsigned long long num, unsigned long long den, unsigned long long * rem_p);

/*
 * 64-bit signed integer modulo.
 */
signed long long __moddi3(signed long long num, signed long long den)
{
	signed int minus = 0;
	signed long long v;

	if (num < 0)
	{
		num = -num;
		minus = 1;
	}
	if (den < 0)
	{
		den = -den;
		minus ^= 1;
	}

	(void)__udivmoddi4(num, den, (unsigned long long *) & v);
	if (minus)
		v = -v;

	return v;
}
