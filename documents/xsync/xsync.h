#ifndef __XSYNC_H__
#define __XSYNC_H__

#include <crc32.h>
#include <sha256.h>
#include <interface.h>
#include <packet.h>

enum xsync_command_t {
	XSYNC_COMMAND_ALIVE		= 0x00,
	XSYNC_COMMAND_START		= 0x01,
	XSYNC_COMMAND_TRANSFER	= 0x02,
	XSYNC_COMMAND_STOP		= 0x03,
	XSYNC_COMMAND_RUN		= 0x04,
	XSYNC_COMMAND_UNKOWN	= 0xff,
};

#endif /* __XSYNC_H__ */
