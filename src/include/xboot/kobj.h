#ifndef __KOBJ_H__
#define __KOBJ_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <list.h>
#include <barrier.h>
#include <atomic.h>
#include <irqflags.h>
#include <spinlock.h>

enum kobj_type_t {
	KOBJ_TYPE_DIR,
	KOBJ_TYPE_REG,
};

struct kobj_t
{
	/* kobj name */
	char * name;

	/* kobj type DIR or REG */
	enum kobj_type_t type;

	/* kobj's parent */
	struct kobj_t * parent;

	/* kobj's entry */
	struct list_head entry;

	/* kobj's children */
	struct list_head children;

	/* kobj lock */
	spinlock_t lock;

	/* kobj read */
	ssize_t (*read)(struct kobj_t * kobj, void * buf, size_t size);

	/* kobj write */
	ssize_t (*write)(struct kobj_t * kobj, void * buf, size_t size);

	/* private data */
	void * priv;
};

typedef ssize_t (*kobj_read_t)(struct kobj_t * kobj, void * buf, size_t size);
typedef ssize_t (*kobj_write_t)(struct kobj_t * kobj, void * buf, size_t size);

struct kobj_t * kobj_get_root(void);
struct kobj_t * kobj_search(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_search_directory_with_create(struct kobj_t * parent, const char * name);
struct kobj_t * kobj_alloc_directory(const char * name);
struct kobj_t * kobj_alloc_regular(const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_free(struct kobj_t * kobj);
bool_t kobj_add(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_remove(struct kobj_t * parent, struct kobj_t * kobj);
bool_t kobj_add_directory(struct kobj_t * parent, const char * name);
bool_t kobj_add_regular(struct kobj_t * parent, const char * name, kobj_read_t read, kobj_write_t write, void * priv);
bool_t kobj_remove_self(struct kobj_t * kobj);

#ifdef __cplusplus
}
#endif

#endif /* __KOBJ_H__ */
