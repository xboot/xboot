#ifndef __CLK_H__
#define __CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
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
	u64_t rate;
};

/*
 * the list of clk
 */
struct clk_list
{
	struct clk * clk;
	struct hlist_node node;
};

bool_t clk_register(struct clk * clk);
bool_t clk_unregister(struct clk * clk);
bool_t clk_get_rate(const char *name, u64_t * rate);

#ifdef __cplusplus
}
#endif

#endif /* __CLK_H__ */
