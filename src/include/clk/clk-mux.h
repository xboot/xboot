#ifndef __CLK_MUX_H__
#define __CLK_MUX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_mux_table_t {
	const char * name;
	int val;
};

struct clk_mux_t {
	const char * name;
	struct clk_mux_table_t * parent;
	int shift;
	int width;
	physical_addr_t phys;
	virtual_addr_t virt;
};

bool_t clk_mux_register(struct clk_mux_t * mclk);
bool_t clk_mux_unregister(struct clk_mux_t * mclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_MUX_H__ */
