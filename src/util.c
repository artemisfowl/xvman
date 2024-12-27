#define _POSIX_X_RESOURCE 200112L
#define _GNU_SOURCE

#include "../inc/util.h"

#include <stdio.h>
#include <unistd.h>

int util_symlink(const char *target, const char *link)
{
	if (!target) {
		fprintf(stderr, "Target path not specified\n");
		return -1;
	}
	if (!link) {
		fprintf(stderr, "Link path not specified\n");
		return -1;
	}

	return symlink(target, link);
}
