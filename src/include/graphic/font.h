#ifndef __GRAPHIC_FONT_H__
#define __GRAPHIC_FONT_H__

#ifdef __cplusplus
extern "C" {
#endif

struct font_t {
	char * family;
	char * style;
	void * face;
};

struct font_t * search_font(const char * family);
void do_loadfont(void);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPHIC_FONT_H__ */
