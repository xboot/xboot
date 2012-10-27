/*
 * __udivdi3.c for 64-bit signed integer divide.
 */

extern unsigned long long __udivmoddi4(unsigned long long num, unsigned long long den, unsigned long long * rem_p);

/*
 * 64-bit unsigned integer divide.
 */
unsigned long long __udivdi3(unsigned long long num, unsigned long long den)
{
	return __udivmoddi4(num, den, 0);
}
