/*
 * __udivmoddi4.c
 */

extern void __div0(void);

/*
 * 64-bit. (internal funtion)
 */
unsigned long long __udivmoddi4(unsigned long long num, unsigned long long den, unsigned long long * rem_p)
{
	unsigned long long quot = 0, qbit = 1;

	if (den == 0)
	{
		return 0;
	}

	/*
	 * left-justify denominator and count shift
	 */
	while ((signed long long) den >= 0)
	{
		den <<= 1;
		qbit <<= 1;
	}

	while (qbit)
	{
		if (den <= num)
		{
			num -= den;
			quot += qbit;
		}
		den >>= 1;
		qbit >>= 1;
	}

	if (rem_p)
		*rem_p = num;

	return quot;
}
