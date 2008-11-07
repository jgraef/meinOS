#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char *progname;

static usage(void)
{
	fprintf(stderr, "Usage: %s name {b|c|p}\n", progname);
	exit(1);
}

int main(int argc, char *argv[])
{
	char *name, *type, typec, *endp;
	unsigned int major, minor;
	mode_t mode;

	progname = *argv++;

	name = *argv++;
	if (!name)
		usage();

	type = *argv++;
	if (!type || !type[0] || type[1])
		usage();
	typec = type[0];

	mode = 0;
	switch (type[0]) {
	case 'c':
		mode = S_IFCHR;
		break;
	case 'b':
		mode = S_IFBLK;
		break;
	case 'p':
		mode = S_IFIFO;
		break;
	default:
		usage();
	}

	if (*argv)
		usage();

	if (mknod(name, mode|0666, 0) == -1) {
		perror("mknod");
		exit(1);
	}

	exit(0);
}
