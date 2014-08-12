#ifndef __CLK_DIVIDER_H__
#define __CLK_DIVIDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum clk_divider_type_t {
	CLK_DIVIDER_ZERO_BASED		= 0x0,
	CLK_DIVIDER_ONE_BASED		= 0x1,
};

struct clk_divider_t {
	const char * name;
	const char * parent;
	virtual_addr_t reg;
	enum clk_divider_type_t type;
	int shift;
	int width;
};

bool_t clk_divider_register(struct clk_divider_t * dclk);
bool_t clk_divider_unregister(struct clk_divider_t * dclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_DIVIDER_H__ */
