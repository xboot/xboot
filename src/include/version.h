#ifndef __VERSION_H__
#define __VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * xboot version
 */
#define	XBOOT_MAJOY			"1"
#define	XBOOT_MINIOR		"5"
#define	XBOOT_PATCH			"0"
#define	XBOOT_VERSION		XBOOT_MAJOY "." XBOOT_MINIOR "." XBOOT_PATCH

const char * xboot_banner_string(void);

#ifdef __cplusplus
}
#endif

#endif /* __VERSION_H__ */
