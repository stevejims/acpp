#ifndef SYS_CLIENT_H
#define SYS_CLIENT_H

#include <stdbool.h>

#include "error.h"
#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <netdb.h>

inline
bool
sys_tryconnect(const addrinfo *ai, int *sock_p)
{
	sys_error_t err;

	/* Attempt to create a socket of the desired type. */
	int sock;
	if (SYS_OK != (err = sys_socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol, &sock))) {
		perror("socket");
		return false;
	}
/*
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	if (0 == getnameinfo(ai->ai_addr, ai->ai_addrlen,
						 hbuf, sizeof (hbuf),
						 sbuf, sizeof (sbuf),
						 NI_NUMERICHOST | NI_NUMERICSERV)) {
		printf("tryconnect: %s:%s\n", hbuf, sbuf);
	}
	else {
		printf("tryconnect: ?\n");
	}
*/
	/* Attempt to connect using the said socket */
	if (0 != connect(sock, ai->ai_addr, ai->ai_addrlen)) {
		perror("connect");
		close(sock);
		return false;
	}

	/* Socket successfully connected. */
	*sock_p = sock;
	return true;
}

inline
sys_error_t
sys_connect(const char *hostname, const char *port, int *sock_p)
{
	/* Construct the hints for discovering the host address. We don't mind attempting connection
	 * via either IPv4 or IPv6 and so the family is unspecified. We do however want to establish
	 * a TCP connection and therefore must specifiy SOCK_STREAM explicitly. */
	const struct addrinfo gai_hints = {
		.ai_flags = AI_NUMERICSERV,
		.ai_family = AF_UNSPEC,
		.ai_socktype = SOCK_STREAM,
		.ai_protocol = 0,
		.ai_addrlen = 0,
		.ai_addr = NULL,
		.ai_canonname = NULL,
		.ai_next = NULL
	};

	struct addrinfo *gai_result = NULL;

	/*  Retrieve the list of potential connections for the requested host. */
	const int gai_error = getaddrinfo(hostname, port, &gai_hints, &gai_result);
	if (gai_error != 0) {
		perror("getaddrinfo");
		return SYS_ESYSCALL_GETADDRINFO;
	}

	/* Iterate over the each address and attempt to connect to it. */
	struct addrinfo *ai;
	for (ai = gai_result; ai != NULL; ai = ai->ai_next) {
		if (sys_tryconnect(ai, sock_p)) {
			break;
		}
	}

	/* Check to see if we failed to connect to anything. */
	const bool connected = (ai != NULL);

	/* Release the result list regardless of success. */
	freeaddrinfo(gai_result);

	/* Socket was either successfully connected or not. */
	return (connected) ? (SYS_OK) : (SYS_NOCONNECT);
}

inline
sys_error_t
sys_connect_local(const char *path, int *sock_p)
{
	sys_error_t err;

	/* Attempt to create a socket of the desired type. */
	int sock;
	if (SYS_OK != (err = sys_socket(AF_UNIX, SOCK_STREAM, 0, &sock))) {
		perror("socket");
		return SYS_NOCONNECT;
	}

	/* Attempt to connect to the said socket */
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
	if (0 != connect(sock, (struct sockaddr *) &addr, sizeof(addr))) {
		perror("connect");
		close(sock);
		return SYS_NOCONNECT;
	}

	/* Socket successfully connected. */
	*sock_p = sock;
	return SYS_OK;
}


#endif /* SYS_CLIENT_H */
