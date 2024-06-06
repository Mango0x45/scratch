#include <sys/wait.h>

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MILLISECOND (1e6)

enum pipe_ends {
	R,
	W,
};

static void *work(void *);

int
main(void)
{
	static const char msg[] = u8"Olá Mundo!\n";

	int fds[2];
	if (pipe(fds) == -1)
		err(1, "pipe");
	if (write(fds[W], msg, sizeof(msg) - 1) == -1)
		err(1, "write[1]");

	pthread_t thrd;
	if ((errno = pthread_create(&thrd, NULL, work, (void *)fds)) != 0)
		err(1, "pthread_create");

	struct timespec ts = {.tv_nsec = 500 * MILLISECOND};
	if (nanosleep(&ts, NULL) == -1)
		err(1, "nanosleep");

	if (write(fds[W], msg, sizeof(msg) - 1) == -1)
		err(1, "write[2]");

	close(fds[W]);

	if ((errno = pthread_join(thrd, NULL)) != 0)
		err(1, "pthread_join");

	return EXIT_SUCCESS;
}

void *
work(void *_fds)
{
	int *fds = _fds;

	struct timespec ts = {.tv_nsec = 250 * MILLISECOND};
	if (nanosleep(&ts, NULL) == -1)
		err(1, "nanosleep");

	pid_t pid = fork();
	if (pid == -1)
		err(1, "fork");
	if (pid == 0) {
		close(STDIN_FILENO);
		if (dup2(fds[R], STDIN_FILENO) == -1)
			err(1, "dup2");
		close(fds[R]);
		close(fds[W]);
		execlp("cat", "cat", NULL);
		err(1, "exec");
	}

	close(fds[R]);
	if (wait(NULL) == -1)
		err(1, "wait");
	return NULL;
}
