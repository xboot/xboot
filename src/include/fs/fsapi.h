#ifndef __FSAPI_H__
#define __FSAPI_H__

#include <configs.h>
#include <default.h>
#include <fs/vfs/fcntl.h>
#include <fs/vfs/stat.h>
#include <fs/vfs/vfs.h>


x_s32 mount(const char * dev, const char * dir, const char * fs, x_u32 flags, void * data);
void sync(void);
x_s32 umount(const char * dir);
x_s32 open(const char * path, x_u32 flags, x_u32 mode);
x_s32 read(x_s32 fd, void * buf, x_size len);
x_s32 write(x_s32 fd, void * buf, x_size len);
x_off lseek(x_s32 fd, x_off offset, x_s32 whence);
x_s32 fstat(x_s32 fd, struct stat * st);
x_s32 ioctl(x_s32 fd, x_u32 cmd, void * arg);
x_s32 fsync(x_s32 fd);
x_s32 close(x_s32 fd);
void * opendir(const char * name);
struct dirent * readdir(void * dir);
x_s32 rewinddir(void * dir);
x_s32 closedir(void * dir);
char * getcwd(char * buf, x_s32 size);
x_s32 chdir(const char * path);
x_s32 mkdir(const char * path, x_u32 mode);
x_s32 rmdir(const char * path);
x_s32 stat(const char * path, struct stat * st);
x_s32 access(const char * path, x_u32 mode);
x_s32 rename(char * oldpath, char * newpath);
x_s32 unlink(const char * path);
x_s32 mknod(const char * path, x_u32 mode);
x_s32 chmod(const char * path, x_u32 mode);
x_s32 chown(const char * path, x_u32 owner, x_u32 group);
x_u32 umask(x_u32 mode);
x_s32 ftruncate(x_s32 fd, x_off length);
x_s32 truncate(const char * path, x_off length);


#endif /* __FSAPI_H__ */
