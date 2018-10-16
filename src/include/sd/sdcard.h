#ifndef __SDCARD_H__
#define __SDCARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <sd/sdhci.h>

void * sdcard_probe(struct sdhci_t * hci);
void sdcard_remove(void * card);

#ifdef __cplusplus
}
#endif

#endif /* __SDCARD_H__ */
