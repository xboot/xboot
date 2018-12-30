/*
 * bcm2837-mbox.c
 *
 * The BCM2837 SoC contains (at least) two CPUs; the VideoCore (a/k/a "GPU")
 * and the ARM CPU. The ARM CPU is often thought of as the main CPU.
 * However, the VideoCore actually controls the initial SoC boot, and hides
 * much of the hardware behind a protocol. This protocol is transported
 * using the SoC's mailbox hardware module.
 *
 * The mailbox hardware supports passing 32-bit values back and forth.
 * Presumably by software convention of the firmware, the bottom 4 bits of the
 * value are used to indicate a logical channel, and the upper 28 bits are the
 * actual payload. Various channels exist using these simple raw messages. See
 * https://github.com/raspberrypi/firmware/wiki/Mailboxes for a list. As an
 * example, the messages on the power management channel are a bitmask of
 * devices whose power should be enabled.
 *
 * The property mailbox channel passes messages that contain the (16-byte
 * aligned) ARM physical address of a memory buffer. This buffer is passed to
 * the VC for processing, is modified in-place by the VC, and the address then
 * passed back to the ARM CPU as the response mailbox message to indicate
 * request completion. The buffers have a generic and extensible format; each
 * buffer contains a standard header, a list of "tags", and a terminating zero
 * entry. Each tag contains an ID indicating its type, and length fields for
 * generic parsing. With some limitations, an arbitrary set of tags may be
 * combined together into a single message buffer. This file defines structs
 * representing the header and many individual tag layouts and IDs.
 *
 * Copyright(c) 2007-2019 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>
#include <bcm2837-mbox.h>
#include <bcm2837/reg-mbox.h>

enum {
	MBOX_CH_POWER_MANAGEMENT	= 0,
	MBOX_CH_FRAMEBUFFER			= 1,
	MBOX_CH_VIRTUAL_UART		= 2,
	MBOX_CH_VCHIQ				= 3,
	MBOX_CH_LEDS				= 4,
	MBOX_CH_BUTTONS				= 5,
	MBOX_CH_TOUCHSCREEN			= 6,
	MBOX_CH_UNUSED				= 7,
	MBOX_CH_TAGS_ARM_TO_VC		= 8,
	MBOX_CH_TAGS_VC_TO_ARM		= 9,
};

static void bcm2837_mbox_write(int channel, uint32_t data)
{
	virtual_addr_t virt = phys_to_virt(BCM2837_MBOX_BASE);

	while((read32(virt + MBOX_STATUS) & (1 << 31)) != 0);
	write32(virt + MBOX_WRITE, (data & ~0xf) | (channel & 0xf));
}

static uint32_t bcm2837_mbox_read(int channel)
{
	virtual_addr_t virt = phys_to_virt(BCM2837_MBOX_BASE);
	uint32_t data;

    do {
    	while((read32(virt + MBOX_STATUS) & (1 << 30)) != 0);
    	data = read32(virt + MBOX_READ);
    } while ((data & 0xf) != channel);

    return (data & ~0xf);
}

static void bcm2837_mbox_call(void * msg)
{
	uint32_t data = 0xC0000000 + virt_to_phys((uint64_t)msg);

	bcm2837_mbox_write(MBOX_CH_TAGS_ARM_TO_VC, data);
	bcm2837_mbox_read(MBOX_CH_TAGS_ARM_TO_VC);
}

/*
 * Mbox videocore
 */
enum {
	MBOX_TAG_VC_GET_FIRMWARE_REV	= 0x00000001,
};

struct mbox_vc_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t val;
	} tag;
	uint32_t end;
};

int bcm2837_mbox_vc_get_firmware_revison(void)
{
	struct mbox_vc_msg_t msg __attribute__((aligned(16)));
	struct mbox_vc_msg_t * p = &msg;

	p->size = sizeof(struct mbox_vc_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_VC_GET_FIRMWARE_REV;
	p->tag.size = 4;
	p->tag.len = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	return p->tag.val;
}

/*
 * Mbox hardware
 */
enum {
	MBOX_TAG_HARDWARE_GET_MODEL 		= 0x00010001,
	MBOX_TAG_HARDWARE_GET_REV 			= 0x00010002,
	MBOX_TAG_HARDWARE_GET_MAC_ADDRESS	= 0x00010003,
	MBOX_TAG_HARDWARE_GET_SERIAL 		= 0x00010004,
	MBOX_TAG_HARDWARE_GET_ARM_MEMORY 	= 0x00010005,
	MBOX_TAG_HARDWARE_GET_VC_MEMORY 	= 0x00010006,
	MBOX_TAG_HARDWARE_GET_CLOCKS 		= 0x00010007,
};

struct mbox_hardware_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t val;
	} tag;
	uint32_t end;
};

struct mbox_hardware_mac_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint8_t val[6];
	} tag;
	uint32_t end;
};

struct mbox_hardware_serial_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint64_t val;
	} tag;
	uint32_t end;
};

struct mbox_hardware_memory_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t mbase;
		uint32_t msize;
	} tag;
	uint32_t end;
};

int bcm2837_mbox_hardware_get_model(void)
{
	struct mbox_hardware_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_MODEL;
	p->tag.size = 4;
	p->tag.len = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_hardware_get_revison(void)
{
	struct mbox_hardware_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_REV;
	p->tag.size = 4;
	p->tag.len = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_hardware_get_mac_address(uint8_t * mac)
{
	struct mbox_hardware_mac_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_mac_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_mac_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_MAC_ADDRESS;
	p->tag.size = 6;
	p->tag.len = 0;
	p->tag.val[0] = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	mac[0] = p->tag.val[0];
	mac[1] = p->tag.val[1];
	mac[2] = p->tag.val[2];
	mac[3] = p->tag.val[3];
	mac[4] = p->tag.val[4];
	mac[5] = p->tag.val[5];
	return 0;
}

int bcm2837_mbox_hardware_get_serial(uint64_t * sn)
{
	struct mbox_hardware_serial_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_serial_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_serial_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_SERIAL;
	p->tag.size = 8;
	p->tag.len = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	*sn = p->tag.val;
	return 0;
}

int bcm2837_mbox_hardware_get_arm_memory(uint32_t * base, uint32_t * size)
{
	struct mbox_hardware_memory_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_memory_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_memory_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_ARM_MEMORY;
	p->tag.size = 8;
	p->tag.len = 0;
	p->tag.mbase = 0;
	p->tag.msize = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	*base = p->tag.mbase;
	*size = p->tag.msize;
	return 0;
}

int bcm2837_mbox_hardware_get_vc_memory(uint32_t * base, uint32_t * size)
{
	struct mbox_hardware_memory_msg_t msg __attribute__((aligned(16)));
	struct mbox_hardware_memory_msg_t * p = &msg;

	p->size = sizeof(struct mbox_hardware_memory_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_HARDWARE_GET_VC_MEMORY;
	p->tag.size = 8;
	p->tag.len = 0;
	p->tag.mbase = 0;
	p->tag.msize = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	*base = p->tag.mbase;
	*size = p->tag.msize;
	return 0;
}

/*
 * Mbox clock
 */
enum {
	MBOX_TAG_CLOCK_GET_TURBO 	= 0x00030009,
	MBOX_TAG_CLOCK_SET_TURBO 	= 0x00038009,
	MBOX_TAG_CLOCK_GET_STATE 	= 0x00030001,
	MBOX_TAG_CLOCK_SET_STATE 	= 0x00038001,
	MBOX_TAG_CLOCK_GET_RATE		= 0x00030002,
	MBOX_TAG_CLOCK_SET_RATE 	= 0x00038002,
	MBOX_TAG_CLOCK_GET_MAX_RATE	= 0x00030004,
	MBOX_TAG_CLOCK_GET_MIN_RATE = 0x00030007,
};

struct mbox_clock_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t id;
		uint32_t val;
	} tag;
	uint32_t end;
};

int bcm2837_mbox_clock_get_turbo(void)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_GET_TURBO;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != 0)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_clock_set_turbo(int level)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_SET_TURBO;
	p->tag.size = 8;
	p->tag.len = 8;
	p->tag.id = 0;
	p->tag.val = level ? 1 : 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != 0)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_clock_get_state(int id)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_GET_STATE;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = id;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return (p->tag.val & 0x3);
}

int bcm2837_mbox_clock_set_state(int id, int state)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_SET_STATE;
	p->tag.size = 8;
	p->tag.len = 8;
	p->tag.id = id;
	p->tag.val = state & 0x3;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return (p->tag.val & 0x3);
}

int bcm2837_mbox_clock_get_rate(int id)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_GET_RATE;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = id;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_clock_set_rate(int id, int rate)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_SET_RATE;
	p->tag.size = 8;
	p->tag.len = 8;
	p->tag.id = id;
	p->tag.val = rate;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_clock_get_max_rate(int id)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_GET_MAX_RATE;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = id;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_clock_get_min_rate(int id)
{
	struct mbox_clock_msg_t msg __attribute__((aligned(16)));
	struct mbox_clock_msg_t * p = &msg;

	p->size = sizeof(struct mbox_clock_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_CLOCK_GET_MIN_RATE;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = id;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return p->tag.val;
}

/*
 * Mbox power
 */
enum {
	MBOX_TAG_POWER_GET_STATE	= 0x00020001,
	MBOX_TAG_POWER_SET_STATE	= 0x00028001,
};

struct mbox_power_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t id;
		uint32_t val;
	} tag;
	uint32_t end;
};

int bcm2837_mbox_power_get_state(int id)
{
	struct mbox_power_msg_t msg __attribute__((aligned(16)));
	struct mbox_power_msg_t * p = &msg;

	p->size = sizeof(struct mbox_power_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_POWER_GET_STATE;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = id;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return (p->tag.val & 0x3);
}

int bcm2837_mbox_power_set_state(int id, int state)
{
	struct mbox_power_msg_t msg __attribute__((aligned(16)));
	struct mbox_power_msg_t * p = &msg;

	p->size = sizeof(struct mbox_power_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_POWER_SET_STATE;
	p->tag.size = 8;
	p->tag.len = 8;
	p->tag.id = id;
	p->tag.val = state & 0x3;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != id)
		return -1;
	return (p->tag.val & 0x3);
}

/*
 * Mbox temperature
 */
enum {
	MBOX_TAG_TEMP_GET		= 0x00030006,
	MBOX_TAG_TEMP_GET_MAX	= 0x0003000A,
};

struct mbox_temp_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t id;
		uint32_t val;
	} tag;
	uint32_t end;
};

int bcm2837_mbox_temp_get(void)
{
	struct mbox_temp_msg_t msg __attribute__((aligned(16)));
	struct mbox_temp_msg_t * p = &msg;

	p->size = sizeof(struct mbox_temp_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_TEMP_GET;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != 0)
		return -1;
	return p->tag.val;
}

int bcm2837_mbox_temp_get_max(void)
{
	struct mbox_temp_msg_t msg __attribute__((aligned(16)));
	struct mbox_temp_msg_t * p = &msg;

	p->size = sizeof(struct mbox_temp_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_TEMP_GET_MAX;
	p->tag.size = 8;
	p->tag.len = 4;
	p->tag.id = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	if(p->tag.id != 0)
		return -1;
	return p->tag.val;
}

/*
 * Mbox framebuffer
 */
enum {
	MBOX_TAG_FB_GET_GPIOVIRT		= 0x00040010,
	MBOX_TAG_FB_ALLOCATE_BUFFER 	= 0x00040001,
	MBOX_TAG_FB_RELEASE_BUFFER 		= 0x00048001,
	MBOX_TAG_FB_BLANK_SCREEN 		= 0x00040002,
	MBOX_TAG_FB_GET_PHYS_WH 		= 0x00040003,
	MBOX_TAG_FB_TEST_PHYS_WH 		= 0x00044003,
	MBOX_TAG_FB_SET_PHYS_WH 		= 0x00048003,
	MBOX_TAG_FB_GET_VIRT_WH 		= 0x00040004,
	MBOX_TAG_FB_TEST_VIRT_WH 		= 0x00044004,
	MBOX_TAG_FB_SET_VIRT_WH 		= 0x00048004,
	MBOX_TAG_FB_GET_DEPTH 			= 0x00040005,
	MBOX_TAG_FB_TEST_DEPTH 			= 0x00044005,
	MBOX_TAG_FB_SET_DEPTH 			= 0x00048005,
	MBOX_TAG_FB_GET_PIXEL_ORDER 	= 0x00040006,
	MBOX_TAG_FB_TEST_PIXEL_ORDER 	= 0x00044006,
	MBOX_TAG_FB_SET_PIXEL_ORDER 	= 0x00048006,
	MBOX_TAG_FB_GET_ALPHA_MODE 		= 0x00040007,
	MBOX_TAG_FB_TEST_ALPHA_MODE 	= 0x00044007,
	MBOX_TAG_FB_SET_ALPHA_MODE 		= 0x00048007,
	MBOX_TAG_FB_GET_PITCH 			= 0x00040008,
	MBOX_TAG_FB_GET_VIRT_OFFSET 	= 0x00040009,
	MBOX_TAG_FB_TEST_VIRT_OFFSET 	= 0x00044009,
	MBOX_TAG_FB_SET_VIRT_OFFSET 	= 0x00048009,
	MBOX_TAG_FB_GET_OVERSCAN 		= 0x0004000a,
	MBOX_TAG_FB_TEST_OVERSCAN 		= 0x0004400a,
	MBOX_TAG_FB_SET_OVERSCAN 		= 0x0004800a,
	MBOX_TAG_FB_GET_PALETTE 		= 0x0004000b,
	MBOX_TAG_FB_TEST_PALETTE 		= 0x0004400b,
	MBOX_TAG_FB_SET_PALETTE 		= 0x0004800b,
};

struct mbox_fb_gpiovirt_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t val;
	} tag;
	uint32_t end;
};

struct mbox_fb_info_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t width;
		uint32_t height;
	} phys;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t width;
		uint32_t height;
	} virt;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t bpp;
	} depth;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t vaddr;
		uint32_t vsize;
	} allocate;
	uint32_t end;
};

struct mbox_fb_offset_msg_t {
	uint32_t size;
	uint32_t code;
	struct {
		uint32_t tag;
		uint32_t size;
		uint32_t len;
		uint32_t xoffset;
		uint32_t yoffset;
	} tag;
	uint32_t end;
};

uint32_t bcm2837_mbox_fb_get_gpiovirt(void)
{
	struct mbox_fb_gpiovirt_msg_t msg __attribute__((aligned(16)));
	struct mbox_fb_gpiovirt_msg_t * p = &msg;

	p->size = sizeof(struct mbox_temp_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_FB_GET_GPIOVIRT;
	p->tag.size = 4;
	p->tag.len = 0;
	p->tag.val = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	return p->tag.val & 0x3fffffff;
}

void * bcm2837_mbox_fb_alloc(int width, int height, int bpp, int nrender)
{
	struct mbox_fb_info_msg_t msg __attribute__((aligned(16)));
	struct mbox_fb_info_msg_t * p = &msg;

	p->size = sizeof(struct mbox_fb_info_msg_t);
	p->code = 0;
	p->phys.tag = MBOX_TAG_FB_SET_PHYS_WH;
	p->phys.size = 8;
	p->phys.len = 8;
	p->phys.width = width;
	p->phys.height = height;
	p->virt.tag = MBOX_TAG_FB_SET_VIRT_WH;
	p->virt.size = 8;
	p->virt.len = 8;
	p->virt.width = width;
	p->virt.height = height * nrender;
	p->depth.tag = MBOX_TAG_FB_SET_DEPTH;
	p->depth.size = 4;
	p->depth.len = 4;
	p->depth.bpp = bpp;
	p->allocate.tag = MBOX_TAG_FB_ALLOCATE_BUFFER;
	p->allocate.size = 8;
	p->allocate.len = 4;
	p->allocate.vaddr = 16;
	p->allocate.vsize = 0;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return 0;
	return (void *)((uint64_t)(p->allocate.vaddr & 0x3fffffff));
}

int bcm2837_mbox_fb_present(int xoffset, int yoffset)
{
	struct mbox_fb_offset_msg_t msg __attribute__((aligned(16)));
	struct mbox_fb_offset_msg_t * p = &msg;

	p->size = sizeof(struct mbox_fb_offset_msg_t);
	p->code = 0;
	p->tag.tag = MBOX_TAG_FB_SET_VIRT_OFFSET;
	p->tag.size = 8;
	p->tag.len = 8;
	p->tag.xoffset = xoffset;
	p->tag.yoffset = yoffset;
	p->end = 0;

	bcm2837_mbox_call(p);
	if(p->code != 0x80000000)
		return -1;
	return 0;
}
