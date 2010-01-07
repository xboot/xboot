#ifndef __CLK_H__
#define __CLK_H__

#include <configs.h>
#include <default.h>
#include <types.h>
#include <string.h>
#include <xboot/list.h>

/*
 * the struct of clk.
 */
struct clk
{
	/* the clk name */
	const char * name;

	/* the clk's rate, HZ */
	x_u64 rate;
};

/*
 * the list of clk
 */
struct clk_list
{
	struct clk * clk;
	struct hlist_node node;
};

x_bool clk_register(struct clk * clk);
x_bool clk_unregister(struct clk * clk);
x_bool clk_get_rate(const char *name, x_u64 * rate);

#endif /* __CLK_H__ */
