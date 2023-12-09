#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <libnotify/notify.h>

#define die(...)  err(EXIT_FAILURE, __VA_ARGS__)
#define diex(...) errx(EXIT_FAILURE, __VA_ARGS__)

static char buf[128];

int
main(int argc, char **argv)
{
	char *endptr;
	long secs;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s secs body\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	errno = 0;
	secs = strtol(argv[1], &endptr, 10);
	if (errno)
		die("strtol: %s", argv[1]);
	if (endptr == argv[1] || *endptr)
		diex("Invalid integer: %s", argv[1]);

	snprintf(buf, sizeof(buf), "%ld Second%s Elapsed", secs,
	         secs == 1 ? "" : "s");

	notify_init("TIMER");
	NotifyNotification *n =
		notify_notification_new(buf, argv[2], "dialog-information");

	notify_notification_set_timeout(n, 5000);
	while ((secs = sleep(secs)))
		sleep(secs);
	if (!notify_notification_show(n, NULL))
		diex("Failed to send notification");

	g_object_unref(G_OBJECT(n));
	notify_uninit();
	return EXIT_SUCCESS;
}
