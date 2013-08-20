#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_t
{
	/* The clk name */
	const char * name;

	/* The clk's rate, HZ */
	u64_t rate;
};

bool_t clk_register(struct clk_t * clk);
bool_t clk_unregister(struct clk_t * clk);
bool_t clk_get_rate(const char * name, u64_t * rate);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */
