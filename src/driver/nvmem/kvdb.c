/*
 * driver/nvmem/kvdb.c
 *
 * Copyright(c) 2007-2018 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <nvmem/kvdb.h>

struct record_t
{
	struct hlist_node node;
	struct list_head head;
	char * key;
	char * value;
};

static char * trim(char * s)
{
	char * e;

	if(s)
	{
		while(isspace(*s))
			s++;
		if(*s == 0)
			return s;
		e = s + strlen(s) - 1;
		while((e > s) && isspace(*e))
			e--;
		*(e + 1) = 0;
	}
	return s;
}

static struct hlist_head * kvdb_hash(struct kvdb_t * db, const char * key)
{
	unsigned char * p = (unsigned char *)key;
	unsigned int seed = 131;
	unsigned int hash = 0;

	while(*p)
	{
		hash = hash * seed + (*p++);
	}
	return &db->hash[hash % db->hash_size];
}

static struct record_t * kvdb_search_record(struct kvdb_t * db, const char * key)
{
	struct record_t * pos;
	struct hlist_node * n;

	if(!db || !key)
		return NULL;

	hlist_for_each_entry_safe(pos, n, kvdb_hash(db, key), node)
	{
		if(strcmp(pos->key, key) == 0)
			return pos;
	}
	return NULL;
}

static bool_t kvdb_add_record(struct kvdb_t * db, struct record_t * r)
{
	irq_flags_t flags;
	int lk, lv;

	if(!db)
		return FALSE;

	if(!r || !r->key || !r->value)
		return FALSE;

	if(kvdb_search_record(db, r->key))
		return FALSE;

	lk = strlen(r->key);
	lv = strlen(r->value);
	if(db->store_size + (lk + lv + 2) > db->max_size)
		return FALSE;

	spin_lock_irqsave(&db->lock, flags);
	init_hlist_node(&r->node);
	hlist_add_head(&r->node, kvdb_hash(db, r->key));
	init_list_head(&r->head);
	list_add_tail(&r->head, &db->list);
	db->store_size += (lk + lv + 2);
	spin_unlock_irqrestore(&db->lock, flags);

	return TRUE;
}

static bool_t kvdb_remove_record(struct kvdb_t * db, struct record_t * r)
{
	irq_flags_t flags;
	int lk, lv;

	if(!db)
		return FALSE;

	if(!r || !r->key || !r->value)
		return FALSE;

	if(hlist_unhashed(&r->node))
		return FALSE;

	lk = strlen(r->key);
	lv = strlen(r->value);
	if(db->store_size - (lk + lv + 2) < 0)
		return FALSE;

	spin_lock_irqsave(&db->lock, flags);
	hlist_del(&r->node);
	list_del(&r->head);
	db->store_size -= (lk + lv + 2);
	spin_unlock_irqrestore(&db->lock, flags);

	return TRUE;
}

struct kvdb_t * kvdb_alloc(int size)
{
	struct kvdb_t * db;
	int i;

	if(size < 0)
		return NULL;

	db = malloc(sizeof(struct kvdb_t));
	if(!db)
		return NULL;

	db->max_size = size;
	db->hash_size = (db->max_size >> 3) + 1;
	if(db->hash_size > CONFIG_KVDB_MAX_HASH_SIZE)
		db->hash_size = CONFIG_KVDB_MAX_HASH_SIZE;
	db->store_size = 1;
	db->hash = malloc(sizeof(struct hlist_head) * db->hash_size);
	if(!db->hash)
	{
		free(db);
		return NULL;
	}

	for(i = 0; i < db->hash_size; i++)
		init_hlist_head(&db->hash[i]);
	init_list_head(&db->list);
	spin_lock_init(&db->lock);

	return db;
}

void kvdb_free(struct kvdb_t * db)
{
	if(db)
	{
		kvdb_clear(db);
		free(db->hash);
		free(db);
	}
}

void kvdb_clear(struct kvdb_t * db)
{
	struct record_t * pos, * n;

	if(!db)
		return;

	list_for_each_entry_safe(pos, n, &db->list, head)
	{
		kvdb_remove_record(db, pos);
		free(pos->key);
		free(pos->value);
		free(pos);
	}
}

void kvdb_set(struct kvdb_t * db, const char * key, const char * value)
{
	struct record_t * r;

	if(!db || !key)
		return;

	if((r = kvdb_search_record(db, key)))
	{
		kvdb_remove_record(db, r);
		free(r->key);
		free(r->value);
		free(r);
	}
	if(value)
	{
		r = malloc(sizeof(struct record_t));
		if(r)
		{
			r->key = strdup(key);
			r->value = strdup(value);
			kvdb_add_record(db, r);
		}
	}
}

char * kvdb_get(struct kvdb_t * db, const char * key, const char * def)
{
	struct record_t * r = kvdb_search_record(db, key);
	if(r)
		return r->value;
	return (char *)def;
}

void kvdb_from_string(struct kvdb_t * db, char * str)
{
	char * p = str;
	char * r, * k, * v;

	if(!db || !p)
		return;

	while((r = strsep(&p, ",;\r\n")) != NULL)
	{
		if(strchr(r, '='))
		{
			k = trim(strsep(&r, "="));
			v = trim(r);
			k = (k && strcmp(k, "") != 0) ? k : NULL;
			v = (v && strcmp(v, "") != 0) ? v : NULL;
			if(k && v)
				kvdb_set(db, k, v);
		}
	}
}

char * kvdb_to_string(struct kvdb_t * db)
{
	struct record_t * pos, * n;
	char * str;
	int len = 0;

	if(!db)
		return NULL;

	str = malloc(db->store_size);
	if(!str)
		return NULL;
	memset(str, 0, db->store_size);

	list_for_each_entry_safe(pos, n, &db->list, head)
	{
		len += sprintf((char *)(str + len), "%s=%s;", pos->key, pos->value);
	}
	return str;
}

int kvdb_summary(struct kvdb_t * db, void * buf)
{
	struct record_t * pos, * n;
	int len = 0;

	if(!db || !buf)
		return 0;

	list_for_each_entry_safe(pos, n, &db->list, head)
	{
		len += sprintf((char *)(buf + len), "%s=%s\r\n", pos->key, pos->value);
	}
	return len;
}
