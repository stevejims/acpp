#ifndef SYS_SERVER_H
#define SYS_SERVER_H

#include "error.h"
#include "socket.h"

#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <errno.h>

inline
bool
sys_trybind(const addrinfo *ai, int *sock_p)
{
	sys_error_t err;

	/* Attempt to create a socket of the desired type. */
	int sock;
	if (SYS_OK != (err = sys_socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol, &sock))) {
		perror("socket");
		return false;
	}

	/* Attempt to bind to the said socket */
	if (0 != bind(sock, ai->ai_addr, ai->ai_addrlen)) {
		perror("bind");
		close(sock);
		return false;
	}

	/* Socket successfully bound. */
	*sock_p = sock;
	return true;
}

inline
sys_error_t
sys_bind(const char *port, int *sock_p)
{
	/* Construct the hints for discovering where to bind our socket locally. */
	const struct addrinfo gai_hints = {
		.ai_flags = AI_PASSIVE,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0,
		.ai_addrlen = 0,
		.ai_addr = NULL,
		.ai_canonname = NULL,
		.ai_next = NULL
	};

	struct addrinfo *gai_result = NULL;

	/* Retrieve the list of potential bind addresses. */
	const int gai_error = getaddrinfo(NULL, port, &gai_hints, &gai_result);
	if (gai_error != 0) {
		perror("getaddrinfo");
		return SYS_ESYSCALL_GETADDRINFO;
	}

	/* Iterate over the each address and attempt to bind to it. */
	struct addrinfo *ai;
	for (ai = gai_result; ai != NULL; ai = ai->ai_next) {
		if (sys_trybind(ai, sock_p)) {
			break;
		}
	}
	
	/* Release the result list regardless of success. */
	freeaddrinfo(gai_result);

	/* Check to see if we failed to bind to anything. */
	if (ai == NULL) {
		return SYS_NOBIND;
	}

	/* Socket was successfully bound. */
	return SYS_OK;
}

inline
sys_error_t
sys_bind_local(const char *path, int *sock_p)
{
	sys_error_t err;

	/* Attempt to create a socket of the desired type. */
	int sock;
	if (SYS_OK != (err = sys_socket(AF_UNIX, SOCK_STREAM, 0, &sock))) {
		perror("socket");
		return SYS_NOBIND;
	}

    /* If the path is a socket, unlink it for re-use. */
    unlink(path);
    
	/* Attempt to bind to the said socket */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
	if (0 != bind(sock, (struct sockaddr *) &addr, sizeof(addr))) {
		perror("bind");
		close(sock);
		return SYS_NOBIND;
	}

	/* Socket successfully bound. */
	*sock_p = sock;
	return SYS_OK;
}

/**
 * Listen on a socket.
 */
inline
sys_error_t
sys_listen(const int listen_sock)
{
	if (0 != listen(listen_sock, 0)) {
		perror("listen");
		return SYS_ESYSCALL_LISTEN;
	}
    return SYS_OK;
}

inline
sys_error_t
sys_accept(const int listen_sock, int *connect_sock)
{
    for (;;) {
        struct sockaddr in_addr;
        socklen_t in_len = sizeof(in_addr);
        const int in_sock = accept(listen_sock, &in_addr, &in_len);
        if (in_sock == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
				continue;
			}
			else {
				perror("accept");
				return SYS_NOBIND;
			}
		}
/*
		char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
		if (0 == getnameinfo(&in_addr, in_len,
							 hbuf, sizeof (hbuf),
							 sbuf, sizeof (sbuf),
							 NI_NUMERICHOST | NI_NUMERICSERV)) {
			printf("Connection Accepted (%s:%s)\n", hbuf, sbuf);
		}
		else {
			printf("Connection Accepted (Unknown)\n");
		}
*/
        *connect_sock = in_sock;
        return SYS_OK;
    }
}

#endif /* SYS_SERVER_H */
