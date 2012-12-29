#ifndef __STREAM_H__
#define __STREAM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>

enum stream_seek {
	STREAM_SEEK_SET	= 0,
	STREAM_SEEK_CUR	= 1,
	STREAM_SEEK_END	= 2,
};

struct stream_t {
	void * priv;

	size_t (*read)(struct stream_t * stream, void * buf, size_t size, size_t count);
	size_t (*write)(struct stream_t * stream, const void * buf, size_t size, size_t count);
	bool_t (*seek)(struct stream_t * stream, loff_t offset, enum stream_seek whence);
	loff_t (*tell)(struct stream_t * stream);
};

struct stream_t * stream_alloc(const char * file, const char * mode);
void stream_free(struct stream_t * stream);

size_t stream_read(struct stream_t * stream, void * buf, size_t size, size_t count);
size_t stream_write(struct stream_t * stream, const void * buf, size_t size, size_t count);
bool_t stream_seek(struct stream_t * stream, loff_t offset, enum stream_seek whence);
loff_t stream_tell(struct stream_t * stream);

#ifdef __cplusplus
}
#endif

#endif /* __STREAM_H__ */
