#ifndef __CLK_LINK_H__
#define __CLK_LINK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_link_t {
	char * name;
	char * parent;
};

bool_t register_clk_link(struct device_t ** device, struct clk_link_t * lclk);
bool_t unregister_clk_link(struct clk_link_t * lclk);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_LINK_H__ */
