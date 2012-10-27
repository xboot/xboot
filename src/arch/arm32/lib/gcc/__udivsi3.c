/*
 * __udivsi3.c for 32-bit unsigned integer divide.
 */

extern unsigned int __udivmodsi4(unsigned int num, unsigned int den, unsigned int * rem_p);

/*
 * 32-bit unsigned integer divide.
 */
unsigned int __udivsi3(unsigned int num, unsigned int den)
{
	return __udivmodsi4(num, den, 0);
}
