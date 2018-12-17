#include <math.h>
#include <xboot/module.h>

double ldexp(double x, int n)
{
	return scalbn(x, n);
}
EXPORT_SYMBOL(ldexp);
