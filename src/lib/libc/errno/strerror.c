/*
 * libc/errno/strerror.c
 */

#include <errno.h>

char * strerror(int e)
{
	switch(e)
	{
	case ENOERR:
		return "No error";
	case EDOM:
		return "Argument outside domain";
	case ERANGE:
		return "Result not representable";
	case ENOSYS:
		return "Function not supported";
	case EINVAL:
		return "Invalid argument";
	case ESPIPE:
		return "Illegal seek";
	case EBADF:
		return "Bad file number";
	case ENOMEM:
		return "Out of memory";
	case EACCES:
		return "Permission denied";
	case ENFILE:
		return "File table overflow";
	case EMFILE:
		return "Too many open files";
	case ENAMETOOLONG:
		return "Filename too long";
	case ELOOP:
		return "Too many levels of symbolic links";
	case ENOMSG:
		return "No message of desired type";
	case E2BIG:
		return "Argument list too long";
	case EINTR:
		return "Interrupted system call";
	case EILSEQ:
		return "Illegal byte sequence";
	case ENOEXEC:
		return "Exec format error";
	case ENOENT:
		return "No such file or directory";
	case EPROTOTYPE:
		return "Protocol wrong type for socket";
	case ESRCH:
		return "No such process";
	case EPERM:
		return "Operation not permitted";
	case ENOTDIR:
		return "Not a directory";
	case ESTALE:
		return "Stale NFS file handle";
	case EISDIR:
		return "Is a directory";
	case EOPNOTSUPP:
		return "Operation not supported on socket";
	case ENOTTY:
		return "Not a tty";
	case EAGAIN:
		return "Operation would block";
	case EIO:
		return "I/O error";
	case ENOSPC:
		return "No space left on device";
	case EEXIST:
		return "File exists";
	case EBUSY:
		return "Device or resource busy";
	case EOVERFLOW:
		return "Value too large for defined data type";
	default:
		break;
	}
	return "Invalid error number";
}
EXPORT_SYMBOL(strerror);
