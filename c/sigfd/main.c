#include <sys/poll.h>
#include <sys/signalfd.h>

#include <err.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define die(...)    err(EXIT_FAILURE, __VA_ARGS__)
#define lengthof(a) (sizeof(a) / sizeof(*(a)))

static char buf[4096];

int
main(void)
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGQUIT);
	sigprocmask(SIG_BLOCK, &set, NULL);

	int fd = signalfd(-1, &set, 0);
	if (fd == -1)
		die("signalfd");

	struct pollfd fds[] = {
		{.fd = fd, .events = POLL_IN},
	};
	for (;;) {
		if (poll(fds, lengthof(fds), -1) == -1)
			die("poll");
		if (fds[0].revents & POLL_IN) {
			fputs("Caught signal!\n", stderr);
			read(fds[0].fd, buf, sizeof(buf));
		}
	}

	return EXIT_SUCCESS;
}
