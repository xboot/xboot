#include <math.h>

#define TBLSIZE 16

static const float
redux = 0x1.8p23f / TBLSIZE,
P1    = 0x1.62e430p-1f,
P2    = 0x1.ebfbe0p-3f,
P3    = 0x1.c6b348p-5f,
P4    = 0x1.3b2c9cp-7f;

static const double exp2ft[TBLSIZE] = {
  0x1.6a09e667f3bcdp-1,
  0x1.7a11473eb0187p-1,
  0x1.8ace5422aa0dbp-1,
  0x1.9c49182a3f090p-1,
  0x1.ae89f995ad3adp-1,
  0x1.c199bdd85529cp-1,
  0x1.d5818dcfba487p-1,
  0x1.ea4afa2a490dap-1,
  0x1.0000000000000p+0,
  0x1.0b5586cf9890fp+0,
  0x1.172b83c7d517bp+0,
  0x1.2387a6e756238p+0,
  0x1.306fe0a31b715p+0,
  0x1.3dea64c123422p+0,
  0x1.4bfdad5362a27p+0,
  0x1.5ab07dd485429p+0,
};

/*
 * exp2f(x): compute the base 2 exponential of x
 *
 * Accuracy: Peak error < 0.501 ulp; location of peak: -0.030110927.
 *
 * Method: (equally-spaced tables)
 *
 *   Reduce x:
 *     x = k + y, for integer k and |y| <= 1/2.
 *     Thus we have exp2f(x) = 2**k * exp2(y).
 *
 *   Reduce y:
 *     y = i/TBLSIZE + z for integer i near y * TBLSIZE.
 *     Thus we have exp2(y) = exp2(i/TBLSIZE) * exp2(z),
 *     with |z| <= 2**-(TBLSIZE+1).
 *
 *   We compute exp2(i/TBLSIZE) via table lookup and exp2(z) via a
 *   degree-4 minimax polynomial with maximum error under 1.4 * 2**-33.
 *   Using double precision for everything except the reduction makes
 *   roundoff error insignificant and simplifies the scaling step.
 *
 *   This method is due to Tang, but I do not use his suggested parameters:
 *
 *      Tang, P.  Table-driven Implementation of the Exponential Function
 *      in IEEE Floating-Point Arithmetic.  TOMS 15(2), 144-157 (1989).
 */
float exp2f(float x)
{
	double_t t, r, z;
	union {float f; uint32_t i;} u = {x};
	union {double f; uint64_t i;} uk;
	uint32_t ix, i0, k;

	/* Filter out exceptional cases. */
	ix = u.i & 0x7fffffff;
	if (ix > 0x42fc0000) {  /* |x| > 126 */
		if (ix > 0x7f800000) /* NaN */
			return x;
		if (u.i >= 0x43000000 && u.i < 0x80000000) {  /* x >= 128 */
			x *= 0x1p127f;
			return x;
		}
		if (u.i >= 0x80000000) {  /* x < -126 */
			if (u.i >= 0xc3160000 || (u.i & 0x0000ffff))
				FORCE_EVAL(-0x1p-149f/x);
			if (u.i >= 0xc3160000)  /* x <= -150 */
				return 0;
		}
	} else if (ix <= 0x33000000) {  /* |x| <= 0x1p-25 */
		return 1.0f + x;
	}

	/* Reduce x, computing z, i0, and k. */
	u.f = x + redux;
	i0 = u.i;
	i0 += TBLSIZE / 2;
	k = i0 / TBLSIZE;
	uk.i = (uint64_t)(0x3ff + k)<<52;
	i0 &= TBLSIZE - 1;
	u.f -= redux;
	z = x - u.f;
	/* Compute r = exp2(y) = exp2ft[i0] * p(z). */
	r = exp2ft[i0];
	t = r * z;
	r = r + t * (P1 + z * P2) + t * (z * z) * (P3 + z * P4);

	/* Scale by 2**k */
	return r * uk.f;
}
EXPORT_SYMBOL(exp2f);
