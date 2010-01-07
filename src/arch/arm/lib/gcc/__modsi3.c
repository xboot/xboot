/*
 * __modsi3.c for 32-bit signed integer modulo.
 */

extern unsigned int __udivmodsi4(unsigned int num, unsigned int den, unsigned int * rem_p);

/**
 * 32-bit signed integer modulo.
 */
signed int __modsi3(signed int num, signed int den)
{
	signed int minus = 0;
	signed int v;

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

	(void)__udivmodsi4(num, den, (unsigned int *) & v);
	if (minus)
		v = -v;

	return v;
}
