#define _GNU_SOURCE
#include <sys/wait.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <stdio.h>
#include <unistd.h>

int
main(void)
{
	int fd, nfd;

	if ((fd = open(_PATH_DEVNULL, O_RDONLY | O_CLOEXEC)) == -1)
		err(1, "open: %s", _PATH_DEVNULL);

	if ((nfd = dup(fd)) == -1)
		err(1, "dup");
	assert((fcntl(nfd, F_GETFD) & FD_CLOEXEC) == 0);
	close(nfd);
	if (dup3(fd, nfd, O_CLOEXEC) == -1)
		err(1, "dup");
	assert((fcntl(nfd, F_GETFD) & FD_CLOEXEC) != 0);

	close(nfd);

	switch (fork()) {
	case -1:
		err(1, "fork");
	case 0:
		puts("If the FD for /dev/null is closed, this should list 4 items");
		fflush(stdout);
		execlp("ls", "ls", "/proc/self/fd", NULL);
		err(1, "exec: true");
	default:
		if (wait(NULL) == -1)
			err(1, "wait");
	}

	/* Not EBADF */
	assert(close(fd) == 0);
}
