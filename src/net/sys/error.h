#ifndef SYS_ERROR_H
#define SYS_ERROR_H

typedef enum sys_error {
	/* Successful call. */
	SYS_OK					= 0,

	/* Error results which are directly due to a system call failing. */
	SYS_ESYSCALL_SOCKET		= -1,
	SYS_ESYSCALL_SOCKETPAIR	= -2,
	SYS_ESYSCALL_FCNTL		= -3,
	SYS_ESYSCALL_FORK		= -4,
	SYS_ESYSCALL_LISTEN		= -5,
	SYS_ESYSCALL_GETADDRINFO= -6,
	SYS_ESYSCALL_SENDMSG	= -7,
	SYS_ESYSCALL_RECV		= -8,
	SYS_ESYSCALL_ACCESS		= -9,
	SYS_ESYSCALL_EPOLLCREATE= -10,
	SYS_ESYSCALL_EPOLLCTL	= -11,
	SYS_ESYSCALL_EPOLLWAIT	= -12,
	SYS_ESYSCALL_EVENTFD	= -13,
	SYS_ESYSCALL_WRITE		= -14,
	SYS_ESYSCALL_READ		= -15,
    SYS_ESYSCALL_SELECT     = -16,

	/* Results which may open to interpretation as to whether they are an error
     * condition or not. These are typiclly not related directly to a failing
     * system call. */
	SYS_EOF					= -101,
	SYS_NOBIND				= -102,
	SYS_NOCONNECT			= -103,
	SYS_EBADCHILDARGS		= -104,
	SYS_EINVALIDSOCKET		= -105,
	SYS_EUNUSABLESOCKET		= -106,
    SYS_FDSETSIZE           = -107,
} sys_error_t;

#endif /* SYS_ERROR_H */
