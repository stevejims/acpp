#ifndef SYS_EPOLL_H
#define SYS_EPOLL_H

#include "error.h"

#include <sys/epoll.h>
#include <unistd.h>

typedef struct sys_epoll {
	int fd;
} sys_epoll_t;

inline
sys_error_t
sys_epoll_init(sys_epoll_t *epoll)
{
	/* Create the epoll file descriptor. */
	epoll->fd = epoll_create1(0);
	if (-1 == epoll->fd) {
		return SYS_ESYSCALL_EPOLLCREATE;
	}

	return SYS_OK;	
}

inline
sys_error_t
sys_epoll_fini(sys_epoll_t *epoll)
{
	/* Close the epoll file descriptor. */
	if (epoll->fd != -1) {
		close(epoll->fd);
	}

	return SYS_OK;	
}

inline
sys_error_t
sys_epoll_add(sys_epoll_t *epoll, const int fd)
{
	/* Construct the epoll event structure. */
	struct epoll_event event = {0};
	event.events = EPOLLIN;
    event.data.fd = fd;

	/* Add the event to the epoll instance. */
	if (0 != epoll_ctl(epoll->fd, EPOLL_CTL_ADD, fd, &event)) {
		return SYS_ESYSCALL_EPOLLCTL;
	}

	return SYS_OK;
}

inline
sys_error_t
sys_epoll_add_ptr(sys_epoll_t *epoll, const int fd, void *data)
{
	/* Construct the epoll event structure. */
	struct epoll_event event = {0};
	event.events = EPOLLIN;
	event.data.ptr = data;

	/* Add the event to the epoll instance. */
	if (0 != epoll_ctl(epoll->fd, EPOLL_CTL_ADD, fd, &event)) {
		return SYS_ESYSCALL_EPOLLCTL;
	}

	return SYS_OK;
}


inline
sys_error_t
sys_epoll_delete(sys_epoll_t *epoll, const int fd)
{
	/* Dummy epoll event structure which will be ignored. */
	struct epoll_event event = {0};

	/* Add the event to the epoll instance. */
	if (0 != epoll_ctl(epoll->fd, EPOLL_CTL_DEL, fd, &event)) {
		return SYS_ESYSCALL_EPOLLCTL;
	}

	return SYS_OK;	
}

inline
sys_error_t
sys_epoll_wait(sys_epoll_t *epoll, int *fd)
{
	/* Wait for any file descriptor in the set to become ready. */
	struct epoll_event event;
	if (1 != epoll_wait(epoll->fd, &event, 1, -1)) {
		return SYS_ESYSCALL_EPOLLWAIT;
	}

	/* Return the data associated with the event. */
    *fd = event.data.fd;

	return SYS_OK;
}

inline
sys_error_t
sys_epoll_wait_ptr(sys_epoll_t *epoll, void **data)
{
	/* Wait for any file descriptor in the set to become ready. */
	struct epoll_event event;
	if (1 != epoll_wait(epoll->fd, &event, 1, -1)) {
		return SYS_ESYSCALL_EPOLLWAIT;
	}

	/* Return the data associated with the event. */
	*data = event.data.ptr;

	return SYS_OK;
}

#endif /* SYS_EPOLL_H */
