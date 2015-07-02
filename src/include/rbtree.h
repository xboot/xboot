#ifndef	__RBTREE_H__
#define	__RBTREE_H__

#include <xboot/module.h>
#include <xboot/list.h>
#include <types.h>
#include <stddef.h>

struct rb_node
{
	unsigned long rb_parent_color;
	struct rb_node * rb_right;
	struct rb_node * rb_left;
} __attribute__((aligned(sizeof(long))));

struct rb_root
{
	struct rb_node * rb_node;
};

typedef void (*rb_augment_f)(struct rb_node * node, void * data);

#define	RB_RED						(0)
#define	RB_BLACK					(1)
#define rb_parent(r)				((struct rb_node *)((r)->rb_parent_color & ~3))
#define rb_color(r)					((r)->rb_parent_color & 1)
#define rb_is_red(r)				(!rb_color(r))
#define rb_is_black(r)				rb_color(r)
#define rb_set_red(r)				do { (r)->rb_parent_color &= ~1; } while (0)
#define rb_set_black(r)				do { (r)->rb_parent_color |= 1; } while (0)
#define RB_ROOT						(struct rb_root){ NULL, }
#define RB_EMPTY_ROOT(root)			((root)->rb_node == NULL)
#define RB_EMPTY_NODE(node)			(rb_parent(node) == node)
#define RB_CLEAR_NODE(node)			(rb_set_parent(node, node))
#define	rb_entry(ptr, type, member)	container_of(ptr, type, member)

static inline void rb_set_parent(struct rb_node * rb, struct rb_node * p)
{
	rb->rb_parent_color = (rb->rb_parent_color & 3) | (unsigned long)p;
}
static inline void rb_set_color(struct rb_node * rb, int color)
{
	rb->rb_parent_color = (rb->rb_parent_color & ~1) | color;
}

static inline void rb_root_init(struct rb_root *root, struct rb_node *node)
{
	root->rb_node = node;
	if (node) {
		node->rb_parent_color = RB_BLACK;
		node->rb_left  = NULL;
		node->rb_right = NULL;
	}
}

static inline void rb_init_node(struct rb_node * rb)
{
	rb->rb_parent_color = 0;
	rb->rb_right = NULL;
	rb->rb_left = NULL;
	RB_CLEAR_NODE(rb);
}

static inline void rb_link_node(struct rb_node * node, struct rb_node * parent, struct rb_node ** rb_link)
{
	node->rb_parent_color = (unsigned long )parent;
	node->rb_left = node->rb_right = NULL;
	*rb_link = node;
}

extern void rb_insert_color(struct rb_node *, struct rb_root *);
extern void rb_erase(struct rb_node *, struct rb_root *);
extern void rb_augment_insert(struct rb_node * node, rb_augment_f func, void * data);
extern struct rb_node *rb_augment_erase_begin(struct rb_node * node);
extern void rb_augment_erase_end(struct rb_node * node, rb_augment_f func, void * data);
extern struct rb_node * rb_next(const struct rb_node *);
extern struct rb_node * rb_prev(const struct rb_node *);
extern struct rb_node * rb_first(const struct rb_root *);
extern struct rb_node * rb_last(const struct rb_root *);
extern void rb_replace_node(struct rb_node * victim, struct rb_node * new, struct rb_root * root);

#endif /* __RBTREE_H__ */
