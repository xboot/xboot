#ifndef __URI_H__
#define __URI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * scheme:[//[user[:pass]@]host[:port]][/path][?query][#fragment]
 */
struct uri_t {
	char * scheme;
	char * userpass;
	char * host;
	int port;
	char * path;
	char * query;
	char * fragment;
};

struct uri_t * uri_alloc(const char * str);
void uri_free(struct uri_t * uri);
int uri_path(struct uri_t * uri, char * buf, int len);
int uri_userpass(struct uri_t * uri, char * user, int ul, char * pass, int pl);

struct uri_query_t {
	const char * key;
	int nkey;
	const char * value;
	int nvalue;
};

int uri_query(const char * query, struct uri_query_t ** info);
void uri_query_free(struct uri_query_t ** info);

#ifdef __cplusplus
}
#endif

#endif /* __URI_H__ */
