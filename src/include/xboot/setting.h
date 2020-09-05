#ifndef __SETTING_H__
#define __SETTING_H__

#ifdef __cplusplus
extern "C" {
#endif

void setting_set(const char * key, const char * value);
const char * setting_get(const char * key, const char * def);
void setting_clear(void);
void setting_summary(void);

void do_init_setting(void);

#ifdef __cplusplus
}
#endif

#endif /* __SETTING_H__ */
