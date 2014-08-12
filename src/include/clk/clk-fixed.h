#ifndef __CLK_FIXED_H__
#define __CLK_FIXED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_fixed_t {
	const char * name;
	u64_t rate;
};

bool_t clk_fixed_register(struct clk_fixed_t * fclk);
bool_t clk_fixed_unregister(struct clk_fixed_t * fclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_FIXED_H__ */
