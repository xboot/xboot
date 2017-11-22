/*
 * driver/nvmem/kvdb.c
 *
 * Copyright(c) 2007-2017 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	if(db->store_size - (lk + lv + 2) < 8)
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

	if(size < 8)
		return NULL;

	db = malloc(sizeof(struct kvdb_t));
	if(!db)
		return NULL;

	db->max_size = size;
	db->hash_size = db->max_size >> 3;
	db->store_size = 8;
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

void kvdb_set(struct kvdb_t * db, char * key, char * value)
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

char * kvdb_get(struct kvdb_t * db, char * key, char * def)
{
	struct record_t * r = kvdb_search_record(db, key);
	if(r)
		return r->value;
	return def;
}

void kvdb_walk(struct kvdb_t * db, void (*cb)(struct kvdb_t * db, char * key, char * value))
{
	struct record_t * pos, * n;

	list_for_each_entry_safe(pos, n, &db->list, head)
	{
		if(cb)
			cb(db, pos->key, pos->value);
	}
}
