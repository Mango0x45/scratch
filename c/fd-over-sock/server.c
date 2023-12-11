#include <sys/socket.h>
#include <sys/un.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define die(...) err(EXIT_FAILURE, __VA_ARGS__)

int
main(void)
{
	int sfd;
	socklen_t saddrlen = sizeof(struct sockaddr_un);
	struct sockaddr_un saddr = {
		.sun_family = AF_UNIX,
		.sun_path = "foo.sock",
	};

	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		die("socket");
	if (bind(sfd, (struct sockaddr *)&saddr, saddrlen) == -1)
		die("bind");
	if (listen(sfd, 4) == -1)
		die("listen");

	for (;;) {
		int cfd = accept(sfd, NULL, NULL);
		if (cfd == -1)
			die("accept");

		struct msghdr msg = {0};
		char m_buffer[sizeof(size_t) * 2];
		struct iovec io = {
			.iov_base = m_buffer,
			.iov_len = sizeof(m_buffer),
		};
		msg.msg_iov = &io;
		msg.msg_iovlen = 1;

		char c_buffer[256];
		msg.msg_control = c_buffer;
		msg.msg_controllen = sizeof(c_buffer);

		if (recvmsg(cfd, &msg, 0) == -1)
			die("recvmsg");

		struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
		unsigned char *data = CMSG_DATA(cmsg);
		int fd = *(int *)data;

		size_t flen, mlen;
		memcpy(&flen, m_buffer, sizeof(size_t));
		memcpy(&mlen, m_buffer + sizeof(size_t), sizeof(size_t));

		// printf("len: %zu\n", msg.msg_iov->iov_len);
		write(STDOUT_FILENO, m_buffer + sizeof(size_t) * 2, mlen);
		putchar('\n');

		char buf[flen];
		read(fd, buf, flen);
		write(STDOUT_FILENO, buf, flen);
		close(cfd);
	}

	return EXIT_SUCCESS;
}
