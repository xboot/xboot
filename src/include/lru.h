#ifndef __LRU_H__
#define __LRU_H__

#ifdef __cplusplus
extern "C" {
#endif

struct lru_item_t {
	struct lru_item_t * next;
	struct lru_item_t * prev;
	struct lru_item_t * hnext;
	int nbytes;
	uint8_t nkey;
	char data[];
};

struct lru_t {
	unsigned int hashpower;
	size_t max_bytes;
	size_t curr_bytes;
	struct lru_item_t ** table;
	struct lru_item_t * head;
	struct lru_item_t * tail;
};

struct lru_t * lru_alloc(size_t maxbytes, unsigned int hashpower);
void lru_free(struct lru_t * l);
int lru_get(struct lru_t * l, const char * key, int nkey, char * buf, int nbuf);
int lru_set(struct lru_t * l, const char * key, int nkey, char * buf, int nbuf);
int lru_remove(struct lru_t * l, const char * key, int nkey);

#ifdef __cplusplus
}
#endif

#endif /* __LRU_H__ */
