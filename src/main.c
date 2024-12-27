#include "../inc/xvman.h"
#include "../inc/log.h"

#include <linux/limits.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	char sname[PATH_MAX];
	char lname[PATH_MAX];
	char values[PATH_MAX];

	bool has_args;
	bool is_present;
	unsigned int argvalc;
} cliopt_t;

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Too few arguments...\n");
		xvman_show_usage();
		return -1;
	}

	xvmanconf_t config;

	if (xvman_setup_prereq(&config)) {
		fprintf(stderr, "Could not setup pre-requisites\n");
		return -1;
	}

	/* setting up the argument type struct instance */
	cliopt_t cli_options[] = {
		{"-a", "--add", "", true, false, 2},
		{"-d", "--debug", "", false, false, 0},
		{"-c", "--config", "", true, false, 1}
	};
	int optc = 3;

	/* this looks extremely ugly but does the work as intended */
	for (int argi = 1; argi <= argc - 1;) {
		for (int optind = 0; optind < optc; ++optind) {
			if ((strcmp(argv[argi], cli_options[optind].sname) == 0) || (strcmp(argv[argi], cli_options[optind].lname) == 0)) {
				if (cli_options[optind].has_args) {
					memset(cli_options[optind].values, '\0', PATH_MAX);
					for (int inc=1; inc <= cli_options[optind].argvalc; ++inc) {
						if (argv[argi+inc]) {
							if (!strlen(cli_options[optind].values))
								strcpy(cli_options[optind].values, argv[argi + inc]);
							else
								strcat(cli_options[optind].values, argv[argi + inc]);

							if (inc < cli_options[optind].argvalc)
								strcat(cli_options[optind].values, " ");
						}
						else {
							fprintf(stderr, "Invalid set of arguments\n");
							return -1;
						}
					}
					argi += cli_options[optind].argvalc == 0 ? 1 : cli_options[optind].argvalc;
				}
				cli_options[optind].is_present = true;
				break;
			}
		}
		argi++;
	}

	unsigned int mode = 0, optind;
	for (int index = 0; index < optc; ++index) {
		if (cli_options[index].is_present) {
			if (strcmp(cli_options[index].sname, "-d") == 0) {
				/* handle debug mode */
				config.debug = true;
				config.enable_flog = true;
				config.enable_slog = true;
			} else if (
				strcmp(cli_options[index].sname, "-a") == 0) {
				/* handle addition mode */
				mode = 100; /* mode for addition */
				optind = index;
			} else if (
				strcmp(cli_options[index].sname, "-c") == 0) {
				/* handle config mode */
				mode = 200; /* mode for config */
				optind = index;
			}
		}
	}

	log_init(config.conf_logfpath, config.debug ? DEBUG : INFO);
	if (config.debug) {
		log_set_stream(config.enable_slog, config.enable_flog);
		debug("Testing a debug log write");
	} else {
		log_set_stream(config.enable_slog, config.enable_flog);
		info("Testing an info log write");
	}

	switch (mode) {
		case 100:
			debug("[add] Values provided: %s",
					cli_options[optind].values);
			xvman_add(cli_options[optind].values);
			break;
		case 200:
			debug("[config] Values provided: %s",
					cli_options[optind].values);
			xvman_config(cli_options[optind].values);
			break;
		default:
			error("Unknown mode set");
			fprintf(stderr, "Unknown mode set\n");
			break;
	}

	xvman_free_mem();

	return 0;
}
