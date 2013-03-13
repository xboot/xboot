/*
 * kernel/core/stream.c
 *
 * Copyright(c) 2007-2013 jianjun jiang <jerryjianjun@gmail.com>
 * official site: http://xboot.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot/stream.h>

static size_t stream_file_read(struct stream_t * stream, void * buf, size_t size, size_t count)
{
    return fread(buf, size, count, stream->priv);
}

static size_t stream_file_write(struct stream_t * stream, const void * buf, size_t size, size_t count)
{
	return fwrite(buf, size, count, stream->priv);
}

static bool_t stream_file_seek(struct stream_t * stream, loff_t offset, enum stream_seek whence)
{
	int flag;

	switch(whence)
	{
	case STREAM_SEEK_SET:
		flag = SEEK_SET;
		break;

	case STREAM_SEEK_CUR:
		flag = SEEK_CUR;
		break;

	case STREAM_SEEK_END:
		flag = SEEK_END;
		break;

	default:
		return FALSE;
	}

	if(fseek(stream->priv, offset, flag) == 0)
		return TRUE;

	return FALSE;
}

static loff_t stream_file_tell(struct stream_t * stream)
{
	return ftell(stream->priv);
}

struct stream_t * stream_alloc(const char * file, const char * mode)
{
	struct stream_t * stream;

	stream = (struct stream_t *)malloc(sizeof(struct stream_t));
	if(! stream)
		return NULL;

	stream->priv = fopen(file, mode);
	if(! stream->priv)
	{
		free(stream);
		return NULL;
	}

	stream->read = stream_file_read;
	stream->write = stream_file_write;
	stream->seek = stream_file_seek;
	stream->tell = stream_file_tell;

	return stream;
}

void stream_free(struct stream_t * stream)
{
	if(!stream)
		return;

	fclose(stream->priv);
	free(stream);
}

size_t stream_read(struct stream_t * stream, void * buf, size_t size, size_t count)
{
	if(stream)
		return stream->read(stream, buf, size, count);
	return 0;
}

size_t stream_write(struct stream_t * stream, const void * buf, size_t size, size_t count)
{
	if(stream)
		return stream->write(stream, buf, size, count);
	return 0;
}

bool_t stream_seek(struct stream_t * stream, loff_t offset, enum stream_seek whence)
{
	if(stream)
		return stream->seek(stream, offset, whence);
	return FALSE;
}

loff_t stream_tell(struct stream_t * stream)
{
	if(stream)
		return stream->tell(stream);
	return -1;
}
