/*
 * __umodsi3.c for 32-bit unsigned integer modulo.
 */

extern unsigned int __udivmodsi4(unsigned int num, unsigned int den, unsigned int * rem_p);

/*
 * 32-bit unsigned integer modulo.
 */
unsigned int __umodsi3(unsigned int num, unsigned int den)
{
	unsigned int v;

	(void)__udivmodsi4(num, den, &v);
	return v;
}
