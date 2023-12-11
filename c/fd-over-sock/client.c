#define _GNU_SOURCE
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>

#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define die(...) err(EXIT_FAILURE, __VA_ARGS__)

typedef uint8_t u8;

int
main(int argc, char **argv)
{
	int fd, sfd;
	void *shm;
	socklen_t saddrlen = sizeof(struct sockaddr_un);
	struct stat sb;
	struct sockaddr_un saddr = {
		.sun_family = AF_UNIX,
		.sun_path = "foo.sock",
	};

	*argv = basename(*argv);
	if (argc != 2) {
		fprintf(stderr, "Usage: %s file\n", *argv);
		exit(EXIT_FAILURE);
	}

	if ((fd = open(argv[1], O_RDONLY)) == -1)
		die("open: %s", argv[1]);
	if (fstat(fd, &sb) == -1)
		die("fstat: %s", argv[1]);
	shm = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (shm == MAP_FAILED)
		die("mmap: %s", argv[1]);

	if ((sfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		die("socket");
	if (connect(sfd, &saddr, saddrlen) == -1)
		die("connect");

	u8 msg[sizeof(size_t) * 2 + sizeof("hello world") - 1];
	char buf[CMSG_SPACE(sizeof(int))];
	struct iovec iov = {
		.iov_base = msg,
		.iov_len = sizeof(msg) - 1,
	};
	struct msghdr hdr = {
		.msg_iov = &iov,
		.msg_iovlen = 1,
		.msg_control = buf,
		.msg_controllen = CMSG_SPACE(sizeof(int)),
	};
	struct cmsghdr *cmsg = CMSG_FIRSTHDR(&hdr);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	cmsg->cmsg_len = CMSG_LEN(sizeof(int));
	*(int *)CMSG_DATA(cmsg) = fd;

	size_t len = sb.st_size;
	size_t len2 = 11;
	memcpy(msg, &len, sizeof(size_t));
	memcpy(msg + sizeof(size_t), &len2, sizeof(size_t));
	memcpy(msg + sizeof(size_t) * 2, "hello, world", 11);

	if (sendmsg(sfd, &hdr, 0) == -1)
		die("sendmsg");

	munmap(shm, sb.st_size);
	close(fd);
	close(sfd);
	return EXIT_SUCCESS;
}
