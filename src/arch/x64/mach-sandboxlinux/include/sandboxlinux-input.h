#ifndef __SANDBOXLINUX_INPUT_H__
#define __SANDBOXLINUX_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <sandboxlinux.h>

struct sandboxlinux_input_data_t
{
	enum input_type_t type;
};

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOXLINUX_INPUT_H__ */
