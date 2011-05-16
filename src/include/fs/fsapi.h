#ifndef __FSAPI_H__
#define __FSAPI_H__

#include <xboot.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>


s32_t mount(const char * dev, const char * dir, const char * fs, u32_t flags);
void sync(void);
s32_t umount(const char * dir);
s32_t open(const char * path, u32_t flags, u32_t mode);
s32_t read(s32_t fd, void * buf, loff_t len);
s32_t write(s32_t fd, void * buf, loff_t len);
loff_t lseek(s32_t fd, loff_t offset, s32_t whence);
s32_t fstat(s32_t fd, struct stat * st);
s32_t ioctl(s32_t fd, u32_t cmd, void * arg);
s32_t fsync(s32_t fd);
s32_t close(s32_t fd);
void * opendir(const char * name);
struct dirent * readdir(void * dir);
s32_t rewinddir(void * dir);
s32_t closedir(void * dir);
char * getcwd(char * buf, s32_t size);
s32_t chdir(const char * path);
s32_t mkdir(const char * path, u32_t mode);
s32_t rmdir(const char * path);
s32_t stat(const char * path, struct stat * st);
s32_t access(const char * path, u32_t mode);
s32_t rename(char * oldpath, char * newpath);
s32_t unlink(const char * path);
s32_t mknod(const char * path, u32_t mode);
s32_t chmod(const char * path, u32_t mode);
s32_t chown(const char * path, u32_t owner, u32_t group);
u32_t umask(u32_t mode);
s32_t ftruncate(s32_t fd, loff_t length);
s32_t truncate(const char * path, loff_t length);


#endif /* __FSAPI_H__ */
