/*
 * __umoddi3.c for 64-bit unsigned integer modulo.
 */

extern unsigned long long __udivmoddi4(unsigned long long num, unsigned long long den, unsigned long long * rem_p);

/*
 * 64-bit unsigned integer modulo.
 */
unsigned long long __umoddi3(unsigned long long num, unsigned long long den)
{
	unsigned long long v;

	(void)__udivmoddi4(num, den, &v);
	return v;
}
