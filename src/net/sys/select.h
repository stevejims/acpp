#ifndef SYS_SELECT_H
#define SYS_SELECT_H

#include "error.h"

#include <cassert>
#include <errno.h>
#include <sys/select.h>
#include <unistd.h>

typedef struct sys_fdset {
	fd_set readfds;
    int maxfd;
} sys_fdset_t;

inline
void
sys_fdset_init(sys_fdset_t *sel)
{
    FD_ZERO(&sel->readfds);
    sel->maxfd = 0;
}

inline
sys_error_t
sys_fdset_add(sys_fdset_t *sel, const int fd)
{
    if (! (fd < FD_SETSIZE)) {
        return SYS_FDSETSIZE;
    }

    FD_SET(fd, &sel->readfds);
    if (fd > sel->maxfd) {
        sel->maxfd = fd;
    }
    return SYS_OK;
}

inline
sys_error_t
sys_fdset_del(sys_fdset_t *sel, const int fd)
{
    if (! (fd < FD_SETSIZE)) {
        return SYS_FDSETSIZE;
    }

    FD_CLR(fd, &sel->readfds);
    return SYS_OK;
}

inline
bool
sys_fdset_readable(const sys_fdset_t *sel, int fd)
{
    assert(fd < FD_SETSIZE);
    return FD_ISSET(fd, &sel->readfds);
}

inline
sys_error_t
sys_select(sys_fdset_t *sel)
{
    for (;;) {
        const int result = select(sel->maxfd + 1, &sel->readfds, 0, 0, 0);
        if (result == -1) {
            if (errno == EINTR) {
                continue;
            }
            else {
                return SYS_ESYSCALL_SELECT;
            }
        }
        return SYS_OK;
    }
}

#endif /* SYS_SELECT_H */
