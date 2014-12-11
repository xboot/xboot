#ifndef __SANDBOX_INPUT_H__
#define __SANDBOX_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <input/input.h>
#include <sandbox.h>

struct sandbox_input_data_t
{
	enum input_type_t type;
};

#ifdef __cplusplus
}
#endif

#endif /* __SANDBOX_INPUT_H__ */
