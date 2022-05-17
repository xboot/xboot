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
	char * user;
	char * pass;
	char * host;
	int port;
	char * path;
	char * query;
	char * fragment;
};

struct uri_t * uri_alloc(const char * s);
void uri_free(struct uri_t * uri);

#ifdef __cplusplus
}
#endif

#endif /* __URI_H__ */
