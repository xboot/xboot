#ifndef __CLK_MUX_H__
#define __CLK_MUX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_mux_table_t {
	char * name;
	int val;
};

struct clk_mux_t {
	char * name;
	struct clk_mux_table_t * parent;
	int shift;
	int width;
	virtual_addr_t virt;
};

bool_t register_clk_mux(struct device_t ** device, struct clk_mux_t * mclk);
bool_t unregister_clk_mux(struct clk_mux_t * mclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_MUX_H__ */
