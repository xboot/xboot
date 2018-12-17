#include <math.h>
#include <xboot/module.h>

float ldexpf(float x, int n)
{
	return scalbnf(x, n);
}
EXPORT_SYMBOL(ldexpf);
