#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

struct clk_t
{
	const char * name;
	int count;

	void (*set_parent)(struct clk_t * clk, const char * name);
	char * (*get_parent)(struct clk_t * clk);
	void (*set_enable)(struct clk_t * clk, bool_t enable);
	bool_t (*get_enable)(struct clk_t * clk);
	void (*set_rate)(struct clk_t * clk, u64_t parent, u64_t rate);
	u64_t (*get_rate)(struct clk_t * clk, u64_t parent);

	void * priv;
};

struct clk_fixed_t {
	const char * name;
	u64_t rate;
};

struct clk_t * clk_search(const char * name);
bool_t clk_register(struct clk_t * clk);
bool_t clk_unregister(struct clk_t * clk);
bool_t clk_fixed_register(struct clk_fixed_t * fclk);
bool_t clk_fixed_unregister(struct clk_fixed_t * fclk);

void clk_enable(const char * name);
void clk_disable(const char * name);
bool_t clk_status(const char * name);
void clk_set_rate(const char * name, u64_t rate);
u64_t clk_get_rate(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */
