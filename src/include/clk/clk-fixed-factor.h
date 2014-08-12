#ifndef __CLK_FIXED_FACTOR_H__
#define __CLK_FIXED_FACTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_fixed_factor_t {
	const char * name;
	const char * parent;
	int mult;
	int div;
};

bool_t clk_fixed_factor_register(struct clk_fixed_factor_t * fclk);
bool_t clk_fixed_factor_unregister(struct clk_fixed_factor_t * fclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_FIXED_FACTOR_H__ */
