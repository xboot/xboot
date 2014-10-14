#include <math.h>

double ldexp(double x, int n)
{
	return scalbn(x, n);
}
EXPORT_SYMBOL(ldexp);
