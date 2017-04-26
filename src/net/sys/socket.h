#ifndef SYS_SOCKET_H
#define SYS_SOCKET_H

#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Set or un-set the close-on-exec flag on a socket.
 */
inline
sys_error_t
sys_socket_set_cloexec(const int sock, const bool set)
{
	const int flags = fcntl(sock, F_GETFD);
	if (-1 == flags) {
		return SYS_ESYSCALL_FCNTL;
	}
	
	const int new_flags = (set)
		? (flags | FD_CLOEXEC)
		: (flags & ~FD_CLOEXEC);
	
	if (-1 == fcntl(sock, F_SETFD, new_flags)) {
		return SYS_ESYSCALL_FCNTL;
	}

	return SYS_OK;
}

/**
 * Create a socket which is close-on-exec by default.
 */
inline
sys_error_t
sys_socket(const int domain, const int type, const int protocol, int *sock_p)
{
	/* Create the unerlying socket of the requrested type. */
	const int sock = socket(domain, type, protocol);
	if (-1 == sock) {
		return SYS_ESYSCALL_SOCKET;
	}

	/* Marked the socket as close-on-exec by default. */
	sys_error_t err;
	if (SYS_OK != (err = sys_socket_set_cloexec(sock, true))) {
		close(sock);
		return err;
	}
	
	/* Return the socket to the caller. */
	*sock_p = sock;
	return SYS_OK;
}

/**
 * Create a pair of connected sockets which are close-on-exec by default.
 */
inline
sys_error_t
sys_socket_pair(const int domain, const int type, const int protocol, int *sock0_p, int *sock1_p)
{
	int sockets[2];
	if (0 != socketpair(domain, type, protocol, sockets)) {
		return SYS_ESYSCALL_SOCKETPAIR;
	}

	/* All sockets are marked as close-on-exec by default. */
	sys_error_t err;
	if ((SYS_OK != (err = sys_socket_set_cloexec(sockets[0], true))) ||
		(SYS_OK != (err = sys_socket_set_cloexec(sockets[1], true)))) {
		close(sockets[0]);
		close(sockets[1]);
		return err;
	}

	/* Return the pair of sockets to the caller. */
	*sock0_p = sockets[0];
	*sock1_p = sockets[1];

	return SYS_OK;
}

#endif /* SYS_SOCKET_H */
