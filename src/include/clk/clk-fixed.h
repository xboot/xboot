#ifndef __CLK_FIXED_H__
#define __CLK_FIXED_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_fixed_t {
	char * name;
	u64_t rate;
};

bool_t register_clk_fixed(struct device_t ** device, struct clk_fixed_t * fclk);
bool_t unregister_clk_fixed(struct clk_fixed_t * fclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_FIXED_H__ */
