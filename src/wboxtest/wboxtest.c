/*
 * wboxtest/wboxtest.c
 */

#include <wboxtest.h>

static struct hlist_head __wboxtest_hash[257];
static spinlock_t __wboxtest_lock = SPIN_LOCK_INIT();

static struct hlist_head * wboxtest_hash(const char * group)
{
	return &__wboxtest_hash[shash(group) % ARRAY_SIZE(__wboxtest_hash)];
}

static struct kobj_t * search_class_wboxtest_group_kobj(const char * group)
{
	struct kobj_t * kclass = kobj_search_directory_with_create(kobj_get_root(), "class");
	struct kobj_t * kwboxtest = kobj_search_directory_with_create(kclass, "wboxtest");
	return kobj_search_directory_with_create(kwboxtest, group);
}

static void wboxtest_run(struct wboxtest_t * wbt, int count)
{
	void * data;
	int i;

	if(wbt && (count > 0))
	{
		wboxtest_print("\033[43;37m[%s]-[%s]\033[0m\r\n", wbt->group, wbt->name);
		data = wbt->setup(wbt);
		for(i = 0; i < count; i++)
		{
			if(count > 1)
				wboxtest_print("\033[44;37m[%d]\033[0m\r\n", i);
			wbt->run(wbt, data);
		}
		wbt->clean(wbt, data);
	}
}

static ssize_t wboxtest_write_test(struct kobj_t * kobj, void * buf, size_t size)
{
	struct wboxtest_t * wbt = (struct wboxtest_t *)kobj->priv;
	int count = strtol(buf, NULL, 0);
	wboxtest_run(wbt, count);
	return size;
}

struct wboxtest_t * search_wboxtest(const char * group, const char * name)
{
	struct wboxtest_t * pos;
	struct hlist_node * n;

	if(!group || !name)
		return NULL;

	hlist_for_each_entry_safe(pos, n, wboxtest_hash(group), node)
	{
		if((strcmp(pos->group, group) == 0) && (strcmp(pos->name, name) == 0))
			return pos;
	}
	return NULL;
}

bool_t register_wboxtest(struct wboxtest_t * wbt)
{
	irq_flags_t flags;

	if(!wbt || !wbt->group || !wbt->name)
		return FALSE;

	if(!wbt->setup || !wbt->clean || !wbt->run)
		return FALSE;

	if(search_wboxtest(wbt->group, wbt->name))
		return FALSE;

	wbt->kobj = kobj_alloc_directory(wbt->name);
	kobj_add_regular(wbt->kobj, "test", NULL, wboxtest_write_test, wbt);
	kobj_add(search_class_wboxtest_group_kobj(wbt->group), wbt->kobj);

	spin_lock_irqsave(&__wboxtest_lock, flags);
	init_hlist_node(&wbt->node);
	hlist_add_head(&wbt->node, wboxtest_hash(wbt->group));
	spin_unlock_irqrestore(&__wboxtest_lock, flags);

	return TRUE;
}

bool_t unregister_wboxtest(struct wboxtest_t * wbt)
{
	irq_flags_t flags;

	if(!wbt || !wbt->group || !wbt->name)
		return FALSE;

	if(hlist_unhashed(&wbt->node))
		return FALSE;

	spin_lock_irqsave(&__wboxtest_lock, flags);
	hlist_del(&wbt->node);
	spin_unlock_irqrestore(&__wboxtest_lock, flags);
	kobj_remove_self(wbt->kobj);

	return TRUE;
}

void wboxtest_run_group_name(const char * group, const char * name, int count)
{
	if(group && name && (count > 0))
		wboxtest_run(search_wboxtest(group, name), count);
}

void wboxtest_run_group(const char * group, int count)
{
	struct wboxtest_t * pos;
	struct hlist_node * n;
	struct slist_t * sl, * e;

	if(group && (count > 0))
	{
		sl = slist_alloc();
		hlist_for_each_entry_safe(pos, n, wboxtest_hash(group), node)
		{
			if((strcmp(pos->group, group) == 0))
				slist_add(sl, pos, "%s", pos->name);
		}
		slist_sort(sl);
		slist_for_each_entry(e, sl)
		{
			wboxtest_run((struct wboxtest_t *)e->priv, count);
		}
		slist_free(sl);
	}
}

void wboxtest_run_all(int count)
{
	struct wboxtest_t * pos;
	struct hlist_node * n;
	struct slist_t * sl, * e;
	int i;

	if(count > 0)
	{
		sl = slist_alloc();
		for(i = 0; i < ARRAY_SIZE(__wboxtest_hash); i++)
		{
			hlist_for_each_entry_safe(pos, n, &__wboxtest_hash[i], node)
			{
				slist_add(sl, pos, "%s%s", pos->group, pos->name);
			}
		}
		slist_sort(sl);
		slist_for_each_entry(e, sl)
		{
			wboxtest_run((struct wboxtest_t *)e->priv, count);
		}
		slist_free(sl);
	}
}

void wboxtest_list(void)
{
	struct wboxtest_t * pos;
	struct hlist_node * n;
	struct slist_t * sl, * e;
	int i;

	sl = slist_alloc();
	for(i = 0; i < ARRAY_SIZE(__wboxtest_hash); i++)
	{
		hlist_for_each_entry_safe(pos, n, &__wboxtest_hash[i], node)
		{
			slist_add(sl, pos, "%s%s", pos->group, pos->name);
		}
	}
	slist_sort(sl);
	slist_for_each_entry(e, sl)
	{
		pos = (struct wboxtest_t *)e->priv;
		wboxtest_print("[%s]-[%s]\r\n", pos->group, pos->name);
	}
	slist_free(sl);
}

int wboxtest_random_int(int a, int b)
{
	double r = (double)rand() * (1.0 / ((double)RAND_MAX + 1.0));
	r *= (double)(b - a) + 1.0;
	return (int)(r + a);
}

double wboxtest_random_float(double a, double b)
{
	double r = (double)rand() * (1.0 / ((double)RAND_MAX + 1.0));
	r *= b - a + 1.0;
	return r + a;
}

char * wboxtest_random_string(char * buf, int len)
{
	char c;
	int i;

	if(buf && len > 0)
	{
		for(i = 0; i < len;)
		{
			c = rand() & 0x7f;
			if(isupper(c) || islower(c))
			{
				buf[i] = c;
				i++;
			}
		}
		buf[len] = '\0';
	}
	return buf;
}

char * wboxtest_random_buffer(char * buf, int len)
{
	int i;

	if(buf && len > 0)
	{
		for(i = 0; i < len; i++)
			buf[i] = rand() & 0xff;
	}
	return buf;
}

int wboxtest_print(const char * fmt, ...)
{
	char buf[SZ_4K];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, SZ_4K, fmt, ap);
	va_end(ap);
	return printf(buf);
}

void wboxtest_assert(int cond, char * expr, const char * file, int line)
{
	int len = wboxtest_print(" [%s:%d] { %s }", file, line, expr);
	wboxtest_print("%*s\r\n", 80 + 12 - 6 - len, cond ? "\033[42;37m[OKAY]\033[0m" : "\033[41;37m[FAIL]\033[0m");
}

static __init void wboxtest_pure_init(void)
{
	int i;

	for(i = 0; i < ARRAY_SIZE(__wboxtest_hash); i++)
		init_hlist_head(&__wboxtest_hash[i]);
}
pure_initcall(wboxtest_pure_init);
