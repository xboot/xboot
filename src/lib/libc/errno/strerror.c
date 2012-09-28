/*
 * libc/errno/strerror.c
 */

#include <xboot/module.h>
#include <errno.h>

/*
 * Returns a string containing a message derived from an error code
 */
char * strerror(int num)
{
	char * p;

	switch(num)
	{
	case ENOERR:
		p = "No error";
		break;

	case EDOM:
		p = "Argument outside domain";
		break;

	case ERANGE:
		p = "Result not representable";
		break;

	case ENOSYS:
		p = "Function not supported";
		break;

	case EINVAL:
		p = "Invalid argument";
		break;

	case ESPIPE:
		p = "Illegal seek";
		break;

	case EBADF:
		p = "Bad file number";
		break;

	case ENOMEM:
		p = "Out of memory";
		break;

	case EACCES:
		p = "Permission denied";
		break;

	case ENFILE:
		p = "File table overflow";
		break;

	case EMFILE:
		p = "Too many open files";
		break;

	case ENAMETOOLONG:
		p = "Filename too long";
		break;

	case ELOOP:
		p = "Too many levels of symbolic links";
		break;

	case ENOMSG:
		p = "No message of desired type";
		break;

	case E2BIG:
		p = "Argument list too long";
		break;

	case EINTR:
		p = "Interrupted system call";
		break;

	case EILSEQ:
		p = "Illegal byte sequence";
		break;

	case ENOEXEC:
		p = "Exec format error";
		break;

	case ENOENT:
		p = "No such file or directory";
		break;

	case EPROTOTYPE:
		p = "Protocol wrong type for socket";
		break;

	case ESRCH:
		p = "No such process";
		break;

	case EPERM:
		p = "Operation not permitted";
		break;

	case ENOTDIR:
		p = "Not a directory";
		break;

	case ESTALE:
		p = "Stale NFS file handle";
		break;

	case EISDIR:
		p = "Is a directory";
		break;

	case EOPNOTSUPP:
		p = "Operation not supported on socket";
		break;

	case ENOTTY:
		p = "Not a tty";
		break;

	case EAGAIN:
		p = "Operation would block";
		break;

	case EIO:
		p = "I/O error";
		break;

	case ENOSPC:
		p = "No space left on device";
		break;

	case EEXIST:
		p = "File exists";
		break;

	case EBUSY:
		p = "Device or resource busy";
		break;

	case EOVERFLOW:
		p = "Value too large for defined data type";
		break;

	default:
		p = "Invalid error number";
		break;
	}

	return p;
}
EXPORT_SYMBOL(strerror);
