#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <err.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define die(...) err(EXIT_FAILURE, __VA_ARGS__)

static void chld(void);
static void prnt(void);

int
main(void)
{
	pid_t pid;
	switch (pid = fork()) {
	case -1:
		die("fork");
	case 0:
		chld();
		break;
	default:
		prnt();
	}
	return EXIT_SUCCESS;
}

void
chld(void)
{
	int fd;

	sleep(1);
	if ((fd = open("foo", O_WRONLY)) == -1)
		die("open: foo");
	if (write(fd, "overwritten", sizeof("overwritten") - 1) == -1)
		die("write");

	close(fd);
}

void
prnt(void)
{
	int fd;
	char *buf;
	struct stat sb;

	if ((fd = open("foo", O_RDONLY)) == -1)
		die("open: foo");
	if (fstat(fd, &sb) == -1)
		die("fstat: foo");
	if ((buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0))
	    == MAP_FAILED)
		die("mmap");

	wait(NULL);
	write(STDOUT_FILENO, buf, sb.st_size);

	munmap(buf, sb.st_size);
	close(fd);
}
