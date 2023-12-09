#include <sys/sendfile.h>
#include <sys/stat.h>

#include <err.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define warn(...) \
	do { \
		warn(__VA_ARGS__); \
		rv = EXIT_FAILURE; \
	} while (0)

static int rv = EXIT_SUCCESS;
static bool out_is_reg;
static char buf[BUFSIZ];

static void cat(const char *);
static void cat_sendfile(const char *, int, size_t);
static void cat_readwrite(const char *, int);

int
main(int argc, char **argv)
{
	struct stat sb;

	if (fstat(STDOUT_FILENO, &sb) == -1)
		warn("fstat: /dev/stdout");
	else
		out_is_reg = S_ISREG(sb.st_mode);

	if (argc == 1)
		cat("-");
	for (int i = 1; i < argc; i++)
		cat(argv[i]);
	return rv;
}

void
cat(const char *file)
{
	int fd;
	struct stat sb;

	if (strcmp(file, "-") == 0)
		fd = STDIN_FILENO;
	else if ((fd = open(file, O_RDONLY)) == -1) {
		warn("open: %s", file);
		return;
	}

	if (fstat(fd, &sb) == -1) {
		warn("fstat: %s", file);
		goto out;
	}

	if (S_ISREG(sb.st_mode) && out_is_reg)
		cat_sendfile(file, fd, sb.st_size);
	else
		cat_readwrite(file, fd);

out:
	close(fd);
}

void
cat_sendfile(const char *file, int fd, size_t n)
{
	off_t off = 0;
	ssize_t nw;

	do {
		nw = sendfile(STDOUT_FILENO, fd, &off, n);
		off += nw;
	} while (nw != -1 && (size_t)off < n);

	if (nw == -1)
		warn("sendfile: %s", file);
}

void
cat_readwrite(const char *file, int fd)
{
	ssize_t nr, nw;

	while ((nr = read(fd, buf, BUFSIZ)) > 0) {
		for (ssize_t off = 0; nr > 0; nr -= nw, off += nw) {
			if ((nw = write(STDOUT_FILENO, buf + off, nr)) == -1) {
				warn("write: %s", file);
				return;
			}
		}
	}
	if (nr == -1)
		warn("read: %s", file);
}
