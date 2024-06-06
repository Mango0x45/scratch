#define _GNU_SOURCE
#include <assert.h>
#include <err.h>
#include <paths.h>
#include <fcntl.h>
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
	close(fd);
}
