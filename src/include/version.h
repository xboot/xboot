#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

int xboot_version(void);
const char * xboot_version_string(void);
const char * xboot_banner_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __VERSION_H__ */
