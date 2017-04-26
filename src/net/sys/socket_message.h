#ifndef SYS_SOCKET_MESSAGE_H
#define SYS_SOCKET_MESSAGE_H

#include <assert.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "error.h"

/**
 * Send a length prefixed message on the given socket. The length is sent as a
 * 32-bit network byte order unsigned interger, immediately followed by the
 * message data of the specified length.
 *
 * \retval 0 Send was successful.
 * \retval -1 Send was unsuccessful. System call error code is left in errno.
 */
inline
sys_error_t
sys_socket_message_send(const int socket,
                        const void *const data,
                        const uint32_t length)
{
	/* Convert the length to 32 bit unsigned network byte order (big endian). */
	const uint32_t net_length = htonl(length);
	
	/* To avoid multiple systems calls, we use an i/o vector to write the length
     * and buffer data at once. The alternative to this would be constructing a
     * new message and copying the data, or using two calls to write/send. */
	const size_t io_count = 2;
	const struct iovec io_req[io_count] = {
		{
			.iov_base = (void *) &net_length,
			.iov_len = sizeof(net_length)
		},
		{
			.iov_base = (void *) data,
			.iov_len = length
		}
	};

	/* To use scatter/gather i/o, we have to use the slightly unintuitive
     * sendmsg interface, which requires a msghdr structure be filled out,
     * though we are not using most of it. */
	const struct msghdr io_hdr = {
		.msg_name = NULL,
		.msg_namelen = 0,
		.msg_iov = (struct iovec *) io_req,
		.msg_iovlen = io_count,
		.msg_control = NULL,
		.msg_controllen = 0,
		.msg_flags = 0
	};

	/* Perform the socket write itself. Note that we request non-blocking send,
     * as we have no interest in its completion. */
	const size_t written_bytes = sendmsg(socket, &io_hdr, MSG_DONTWAIT); 
	if (written_bytes == -1) {
		return SYS_ESYSCALL_SENDMSG;
	}

	/* The written count should matche the sum of the sizes specified in the
     * i/o request. */
	assert(written_bytes == (io_req[0].iov_len + io_req[1].iov_len));

	return SYS_OK;
}

/**
 * Receive the a length field of a length prefixed message on the given socket.
 * The length is received as a 32-bit network byte order unsigned interger.
 *
 * \retval 0 Receive was successful.
 * \retval -1 Receive was unsuccessful. System call error code is left in errno.
 */
inline
sys_error_t
sys_socket_message_recv_length(const int socket, uint32_t *const length)
{
	uint32_t net_length = 0;

	/* Receive the 32-bit network byte order length. Paranoid check that the
     * amount of data we received was exactly as expected, otherwise it is
     * unlikely we can recover. */
	const size_t result = recv(socket,
                               (void *) &net_length,
                               sizeof(net_length),
                               MSG_WAITALL);
	if (result == 0) {
		return SYS_EOF;
	}
	if (result != sizeof(net_length)) {
		return SYS_ESYSCALL_RECV;
	}

	/* Convert the network byte ordered value to host order for returning. */
	*length = ntohl(net_length);

	return SYS_OK;
}

/**
 * Receive the data component of a length prefixed message on the given socket.
 *
 * \retval 0 Receive was successful.
 * \retval -1 Receive was unsuccessful. System call error code is left in errno.
 */
inline
sys_error_t
sys_socket_message_recv_data(const int socket,
                             void *const message,
                             const uint32_t length)
{
	/* Receive the full expected length of data. Paranoid check that the amount
     * of data we received was exactly as expected, otherwise it is unlikely we
     * can recover. */
	const size_t result = recv(socket, message, length, MSG_WAITALL);
	if (result == 0) {
		return SYS_EOF;
	}
	if (result != length) {
		return SYS_ESYSCALL_RECV;
	}

	return SYS_OK;
}

#endif /* SYS_SOCKET_MESSAGE_H */
