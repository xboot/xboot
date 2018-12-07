#ifndef	__RBTREE_H__
#define	__RBTREE_H__

#include <xboot/module.h>
#include <types.h>
#include <list.h>
#include <stddef.h>

struct rb_node {
	unsigned long  __rb_parent_color;
	struct rb_node * rb_right;
	struct rb_node * rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root {
	struct rb_node * rb_node;
};

struct rb_root_cached {
	struct rb_root rb_root;
	struct rb_node * rb_leftmost;
};

#define rb_parent(r)   				((struct rb_node *)((r)->__rb_parent_color & ~3))
#define RB_ROOT						(struct rb_root) { NULL, }
#define RB_ROOT_CACHED				(struct rb_root_cached) { {NULL, }, NULL }
#define RB_EMPTY_ROOT(root)  		((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)			((node)->__rb_parent_color == (unsigned long)(node))
#define RB_CLEAR_NODE(node)			((node)->__rb_parent_color = (unsigned long)(node))
#define	rb_entry(ptr, type, member)	container_of(ptr, type, member)
#define rb_first_cached(root)		(root)->rb_leftmost

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);
extern struct rb_node * rb_next(const struct rb_node *);
extern struct rb_node * rb_prev(const struct rb_node *);
extern struct rb_node * rb_first(const struct rb_root *);
extern struct rb_node * rb_last(const struct rb_root *);
extern void rb_insert_color_cached(struct rb_node *, struct rb_root_cached *, int);
extern void rb_erase_cached(struct rb_node *, struct rb_root_cached *);
extern struct rb_node * rb_first_postorder(const struct rb_root *);
extern struct rb_node * rb_next_postorder(const struct rb_node *);
extern void rb_replace_node(struct rb_node * victim, struct rb_node * new, struct rb_root * root);
extern void rb_replace_node_cached(struct rb_node * victim, struct rb_node * new, struct rb_root_cached * root);

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent, struct rb_node ** rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->rb_left = node->rb_right = NULL;
	*rb_link = node;
}

#define rb_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? rb_entry(____ptr, type, member) : NULL; \
	})

#define rbtree_postorder_for_each_entry_safe(pos, n, root, field) \
	for (pos = rb_entry_safe(rb_first_postorder(root), typeof(*pos), field); \
	     pos && ({ n = rb_entry_safe(rb_next_postorder(&pos->field), \
			typeof(*pos), field); 1; }); \
	     pos = n)

#endif /* __RBTREE_H__ */
