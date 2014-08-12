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

	void (*set_parent)(struct clk_t * clk, const char * pname);
	const char * (*get_parent)(struct clk_t * clk);
	void (*set_enable)(struct clk_t * clk, bool_t enable);
	bool_t (*get_enable)(struct clk_t * clk);
	void (*set_rate)(struct clk_t * clk, u64_t prate, u64_t rate);
	u64_t (*get_rate)(struct clk_t * clk, u64_t prate);

	void * priv;
};

struct clk_fixed_t {
	const char * name;
	u64_t rate;
};

struct clk_fixed_factor_t {
	const char * name;
	const char * parent;
	int mult;
	int div;
};

struct clk_gate_t {
	const char * name;
	const char * parent;
	virtual_addr_t reg;
	int shift;
	int invert;
};

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

struct clk_mux_table_t {
	const char * name;
	int val;
};

struct clk_mux_t {
	const char * name;
	struct clk_mux_table_t * parent;
	virtual_addr_t reg;
	int shift;
	int width;
};

struct clk_pll_t {
	const char * name;
	const char * parent;
	void (*set_rate)(struct clk_pll_t * pclk, u64_t prate, u64_t rate);
	u64_t (*get_rate)(struct clk_pll_t * pclk, u64_t prate);
};

struct clk_t * clk_search(const char * name);
bool_t clk_register(struct clk_t * clk);
bool_t clk_unregister(struct clk_t * clk);
bool_t clk_fixed_register(struct clk_fixed_t * fclk);
bool_t clk_fixed_unregister(struct clk_fixed_t * fclk);
bool_t clk_fixed_factor_register(struct clk_fixed_factor_t * fclk);
bool_t clk_fixed_factor_unregister(struct clk_fixed_factor_t * fclk);
bool_t clk_gate_register(struct clk_gate_t * gclk);
bool_t clk_gate_unregister(struct clk_gate_t * gclk);
bool_t clk_divider_register(struct clk_divider_t * dclk);
bool_t clk_divider_unregister(struct clk_divider_t * dclk);
bool_t clk_mux_register(struct clk_mux_t * mclk);
bool_t clk_mux_unregister(struct clk_mux_t * mclk);
bool_t clk_pll_register(struct clk_pll_t * pclk);
bool_t clk_pll_unregister(struct clk_pll_t * pclk);

void clk_set_parent(const char * name, const char * pname);
const char * clk_get_parent(const char * name);
void clk_enable(const char * name);
void clk_disable(const char * name);
bool_t clk_status(const char * name);
void clk_set_rate(const char * name, u64_t rate);
u64_t clk_get_rate(const char * name);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */
