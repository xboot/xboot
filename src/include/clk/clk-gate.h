#ifndef __CLK_GATE_H__
#define __CLK_GATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_gate_t {
	char * name;
	char * parent;
	int shift;
	int invert;
	virtual_addr_t virt;
	void * priv;
};

bool_t register_clk_gate(struct device_t ** device, struct clk_gate_t * gclk);
bool_t unregister_clk_gate(struct clk_gate_t * gclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_GATE_H__ */
