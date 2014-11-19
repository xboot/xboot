#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	XBOOT_MAJOY			1
#define	XBOOT_MINIOR		6
#define	XBOOT_PATCH			0

int xboot_version(void);
const char * xboot_version_string(void);
const char * xboot_banner_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __VERSION_H__ */
