#ifndef __URI_H__
#define __URI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * scheme:[//[user[:password]@]host[:port]][/path][?query][#fragment]
 */
struct uri_t {
	char * scheme;
	char * user;
	char * pass;
	char * host;
	char * port;
	char * path;
	char * query;
	char * fragment;
	char buf[2083 + 1 + 1];
};

int uri_parse(const char * s, struct uri_t * uri);

#ifdef __cplusplus
}
#endif

#endif /* __URI_H__ */
