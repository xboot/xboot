#ifndef __FILEIO_H__
#define __FILEIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <xboot.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>

struct iovec {
	void * iov_base;
	size_t iov_len;
};

int mount(const char * dev, const char * dir, const char * fs, u32_t flags);
void sync(void);
int umount(const char * dir);
int open(const char * path, u32_t flags, u32_t mode);
loff_t read(int fd, void * buf, loff_t len);
loff_t write(int fd, void * buf, loff_t len);
loff_t lseek(int fd, loff_t offset, s32_t whence);
int fstat(int fd, struct stat * st);
int ioctl(int fd, int cmd, void * arg);
int fsync(int fd);
int close(int fd);
void * opendir(const char * name);
struct dirent * readdir(void * dir);
int rewinddir(void * dir);
int closedir(void * dir);
char * getcwd(char * buf, size_t size);
int chdir(const char * path);
int mkdir(const char * path, u32_t mode);
int rmdir(const char * path);
int stat(const char * path, struct stat * st);
int access(const char * path, u32_t mode);
int rename(char * oldpath, char * newpath);
int unlink(const char * path);
int mknod(const char * path, u32_t mode);
int chmod(const char * path, u32_t mode);
int chown(const char * path, u32_t owner, u32_t group);
u32_t umask(u32_t mode);
int ftruncate(int fd, loff_t length);
int truncate(const char * path, loff_t length);

ssize_t readv(int fd, const struct iovec * iov, int iovcnt);
ssize_t writev(int fd, const struct iovec * iov, int iovcnt);

#ifdef __cplusplus
}
#endif

#endif /* __FILEIO_H__ */
