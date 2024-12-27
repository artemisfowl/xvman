/**
 * @file xvman.c
 * @brief File containing program specific sub-routines.
 *
 * @date Sat, 07 Dec 2024 08:48:41 +0530
 * @bug No known bugs.
 */

#define _GNU_SOURCE
#include "../inc/xvman.h"
#include "../inc/log.h"
#include "../inc/io.h"
#include "../inc/util.h"

#include <linux/limits.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void xvman_show_usage()
{
	printf("xvman [OPTIONS [VALUES]]\n");
}

void xvman_free_mem(void)
{
	info("Freeing up all the allocated memory");
	log_free_lf();
	printf("Exiting...\n");
}

int xvman_setup_prereq(xvmanconf_t *config)
{
	if (!config) {
		fprintf(stderr, "XVMAN configuration struct instance "
				"not provided\n");
		return -1;
	}

	/* clear up the memory and then setup the path properly */
	memset(config->cbin, '\0', PATH_MAX);
	memset(config->confdir, '\0', PATH_MAX);
	memset(config->conf_fpath, '\0', PATH_MAX);
	memset(config->conf_logfpath, '\0', PATH_MAX);

	/* setup the configuration directory path */
	sprintf(config->cbin, "%s/%s", getenv("HOME"), CBIN);
	sprintf(config->confdir, "%s/%s", getenv("HOME"), CONFDIR);
	sprintf(config->conf_fpath, "%s/%s", getenv("HOME"), CONF_FPATH);
	sprintf(config->conf_logfpath, "%s/%s", getenv("HOME"), CONF_LOGFPATH);

	/* create the directory and the configuration file */
	if (io_mkdir(config->confdir, S_IRWXU, true)) {
		fprintf(stderr, "Error while setting up config directory: %s\n",
				config->confdir);
		return -1;
	}
	if (!io_path_exists(config->conf_fpath)) {
		FILE *conf_file = fopen(config->conf_fpath, "w");
		if (!conf_file) {
			fprintf(stderr, "Error while creating "
					"configuration file at location: %s\n",
					config->conf_fpath);
			return -1;
		}
		fclose(conf_file);
	}
	if (io_mkdir(config->cbin, S_IRWXU, true)) {
		fprintf(stderr, "Error while trying to create "
				"custom binary directory\n");
		return -1;
	}

	/*
	 * update the path to include custom binary location.
	 *
	 * NOTE:
	 * As of now, the following portion will assume that BASH is default
	 * shell.
	 */
	char bash_cfilepath[PATH_MAX], export[PATH_MAX];
	memset(bash_cfilepath, '\0', PATH_MAX);
	memset(export, '\0', PATH_MAX);
	int result = 0;
	if ((result = sprintf(bash_cfilepath,
				"%s/%s", getenv("HOME"),
				BASH_CONF_FILE)) < 0) {
		fprintf(stderr, "Error while preparing the "
				"bash configuration filepath\n");
		return -1;
	}
	if ((result = sprintf(export, "\nexport %s\n", RCUPDATE)) < 0) {
		fprintf(stderr, "Error while updating "
				"configuration file update content\n");
		return -1;
	}

	/* create the lock file inside the xvman config directory so that no
	 * more updation of the bash configuration file is done */
	char lockfilepath[PATH_MAX];
	memset(lockfilepath, '\0', PATH_MAX);
	if ((result = sprintf(lockfilepath, "%s/%s",
					getenv("HOME"), LOCKFILE)) < 0) {
		fprintf(stderr, "Error while updating lock filepath\n");
		return -1;
	}

	/*
	 * Note:
	 * If the lockfile does not exist, update the bash configuration file
	 * and then create the lockfile.
	 */
	if (!io_path_exists(lockfilepath)) {
		printf("\n\n"
			"BASH configuration file for USER: %s has been updated\n"
			"In case the default shell is not BASH, \nplease update the"
			" respective shell configuration file with the following "
			"content:\n%s"
			"\n\n", getenv("USER"), RCUPDATE);
		/* update the bash configuration file */
		FILE *bash_cfile = fopen(bash_cfilepath, "a+");
		fwrite(export, sizeof(char), strlen(export), bash_cfile);
		if (bash_cfile) {
			if (fclose(bash_cfile)) {
				fprintf(stderr, "Unable to close bash "
					"configuration file "
					"handle post update\n");
				return -1;
			}
		}

		/* create the lockfile now */
		FILE *lockfile = fopen(lockfilepath, "w");
		if (lockfile) {
			if (fclose(bash_cfile)) {
				fprintf(stderr, "Unable to close the lock file"
						" handler\n");
				return -1;
			}
		}
	}


	/* debug mode is disabled by default */
	config->debug = false;

	/* enable logging to file, disabling stream by default */
	config->enable_flog = true;
	config->enable_slog = false;

	return 0;
}

int xvman_add(const char *data)
{
	if (!data) {
		error("Data having program name and "
				"install location not specified");
		fprintf(stderr, "Data containing program name and "
				"install location not provided\n");
		return -1;
	}

	info("About to add the specified program and install location");
	debug("Data provided : %s", data);

	/*
	 * Note:
	 * The data will contain the name of the program and the install
	 * location separated by a space. Tokenize and perform the operations.
	 */
	int index = 0;
	char pname[100], ilocation[200];
	memset(pname, '\0', 100);
	memset(ilocation, '\0', 100);
	for (char *token = strtok((char *)data, " ");
			token; token = strtok(NULL, " "), index++) {
		if (index == 0)
			strcpy(pname, token);
		else if (index == 1)
			strcpy(ilocation, token);
	}

	debug("Program: %s, install location: %s", pname, ilocation);

	/*
	 * Note:
	 * check if the configuration file is created or not, if not return
	 * with the -1, else write the data into the respective xvman
	 * configuration, the program configuration and create the symlink
	 * accordingly. Delete the older symlink if one already exists.
	 */
	if (!io_path_exists(ilocation)) {
		error("Install location specified does not exist");
		fprintf(stderr, "\nError: \nInstall location "
				"specified does not exist\n\n");
		return -1;
	}

	/* if the path exists, start the processing */
	char pconfig[PATH_MAX];
	memset(pconfig, '\0', PATH_MAX);
	int result = 0;
	if ((result = sprintf(pconfig, "%s/%s/%s",
				getenv("HOME"), CONFDIR, pname)) < 0) {
		fprintf(stderr, "Error while setting up "
				"program config file path\n");
		return -1;
	}
	debug("Program config file path: %s", pconfig);

	if (!io_path_exists(pconfig)) {
		debug("Program configuration file does not exist");
		/*
		 * Note:
		 * If the program configuration file does not exist, create it
		 * and fill it up with the right path.
		 */

		/* since the file does not exist, create it and add the install
		 * location */
		FILE *pconf_file = fopen(pconfig, "w");
		fwrite(ilocation, sizeof(char), strlen(ilocation), pconf_file);
		fwrite("\n", sizeof(char), strlen("\n"), pconf_file);
		if (pconf_file)
			fclose(pconf_file);
	} else {
		debug("Program configuration file already exists");

		/*
		 * Note:
		 * If the program configuration file exists, read the contents
		 * and save them. Post that, put the recently added install
		 * location at the top of the file and then add the contents
		 * under the new install location.
		 */

		FILE *r_pconf_file = fopen(pconfig, "r");
		char *line = NULL;
		size_t len = 0, read;

		while ((read = getline(&line, &len, r_pconf_file)) != -1) {
			line[read-1] = line[read-1] == '\n' ? '\0' : line[read-1];
			debug("Line content: %s, length: %zu", line, read);
			if (strcmp(ilocation, line) ==  0) {
				warning("Location: %s already added",
						ilocation);
				fprintf(stderr, "Location %s already added\n",
						ilocation);
				if (r_pconf_file)
					fclose(r_pconf_file);
				free(line);
				return -1;
			}
		}
		if (r_pconf_file)
			fclose(r_pconf_file);

		free(line);

		/* read the file line by line */
		FILE *read_pconf_file = fopen(pconfig, "r");
		FILE *w_tmp_file = fopen("./tmp", "a");

		line = NULL;
		len = 0, read = 0;


		while ((read = getline(&line, &len, read_pconf_file)) != -1 ) {
			if (line[strlen(line)-1] == '\n')
				line[strlen(line)-1] = '\0';
			debug("Retrieved line of length: %zu, "
					"line content: %s",
					read, line);
			fwrite(line, sizeof(char), strlen(line), w_tmp_file);
			fwrite("\n", sizeof(char), strlen("\n"), w_tmp_file);
		}

		if (read_pconf_file)
			fclose(read_pconf_file);
		if (w_tmp_file)
			fclose(w_tmp_file);

		free(line);

		/*
		 * Note:
		 * Now read the tmp file and re-populate the existing program
		 * configuration file.
		 */

		FILE *write_pconf_file = fopen(pconfig, "w");
		FILE *r_tmp_file = fopen("./tmp", "r");

		line = NULL;
		len = 0, read = 0;

		fwrite(ilocation, sizeof(char), strlen(ilocation),
				write_pconf_file);
		fwrite("\n", sizeof(char), strlen("\n"), write_pconf_file);
		while ((read = getline(&line, &len, r_tmp_file)) != -1) {
			debug("Writing line content: %s, length: %zu",
					line, read);
			fwrite(line, sizeof(char), strlen(line),
					write_pconf_file);
		}

		if (write_pconf_file)
			fclose(write_pconf_file);
		if (r_tmp_file)
			fclose(r_tmp_file);

		/* delete the tmp file */
		debug("Cleaning up the tmp file...");
		remove("./tmp");

		free(line);
	}

	char slink[PATH_MAX];
	memset(slink, '\0', PATH_MAX);
	if ((result = sprintf(slink, "%s/%s/%s",
				getenv("HOME"), CBIN, pname)) < 0) {
		error("Error while forming symlink path");
		fprintf(stderr, "Error while forming symlink path\n");
		return -1;
	}
	debug("Symlink path: %s", slink);

	if (!io_path_exists(slink)) {
		info("Symlink is not present, creating it...");
	} else {
		info("Symlink is present, deleting and recreating...");
		/*
		 * Note:
		 * Delete the existing symlink file and recreate the new one
		 * and then recreate the symlink file.
		 */
		remove(slink);
	}

	if (util_symlink(ilocation, slink)) {
		error("Error while creating symlink");
		fprintf(stderr, "Error while creating symlink\n");
		return -1;
	}

	return 0;
}

int xvman_config(const char *pname)
{
	if (!pname) {
		error("Program name not specified");
		fprintf(stderr, "Program name not specified\n");
		return -1;
	}

	info("About to configure the version...");

	char pconfig_file[PATH_MAX];
	memset(pconfig_file, '\0', PATH_MAX);
	int result;
	if ((result = sprintf(pconfig_file, "%s/%s/%s", getenv("HOME"),
					CONFDIR, pname)) < 0) {
		error("Error while preparing program configuration file\n");
		fprintf(stderr, "Error while preparing program "
				"configuration file\n");
		return -1;
	}
	debug("Program configuration file path: %s", pconfig_file);

	if (!io_path_exists(pconfig_file)) {
		error("Program: %s is not configured", pconfig_file);
		fprintf(stderr, "Program: %s is not configured\n",
				pconfig_file);
		return -1;
	}

	/* since the path is present read the contents of the file as well as
	 * show the same to the user */
	FILE *r_pconf_file = fopen(pconfig_file, "r");

	/* get the line count in the configuration file */
	char *line = NULL;
	size_t len = 0, read = 0, lc = 0;
	while ((read = getline(&line, &len, r_pconf_file)) != -1)
		lc++;
	free(line);

	len = 0;
	fseek(r_pconf_file, 0, 0);
	char **locations = calloc(sizeof(char *), lc);
	size_t location_index = 0;
	while ((read = getline(&line, &len, r_pconf_file)) != -1) {
		debug("Install location: %s, length: %zu", line, read);
		printf("%zu. %s", location_index+1, line);
		locations[location_index] = calloc(sizeof(char), read+1);
		if ((result = sprintf(
				locations[location_index], "%s", line)) < 0) {
			error("Error while storing install location");
			break;
		}
		location_index++;
	}
	free(line);
	if (r_pconf_file)
		fclose(r_pconf_file);

	int choice = -1;
	bool write = false;
	printf("Please enter your choice: ");
	scanf("%d", &choice);
	if ((choice < 0) || (choice > lc)) {
		error("Invalid choice provided");
		fprintf(stderr, "\nInvalid choice provided\n");
		write = false;
	} else
		write = true;

	/* fixme: this needs to be used for re-writing into the file */
	if (write) {
		debug("Install location chosen: %s", locations[choice - 1]);
		printf("Install location chosen: %s", locations[choice - 1]);
		FILE *w_pconf_file = fopen(pconfig_file, "w");
		if (!w_pconf_file) {
			error("Error while opening "
				"program config file for updating");
			fprintf(stderr, "Error while opening "
				"program config file for updating\n");
			for (size_t i = 0; i < lc; i++)
				free(locations[i]);
			if (locations)
				free(locations);
			return -1;
		}
		fwrite(locations[choice-1], sizeof(char),
			strlen(locations[choice-1]), w_pconf_file);
		for (size_t i = 0; i < lc; ++i) {
			if (i != (choice-1)) {
				debug("Appending stored location: %s",
						locations[i]);
				fwrite(locations[i], sizeof(char),
					strlen(locations[i]), w_pconf_file);
			}
		}
		if (w_pconf_file)
			fclose(w_pconf_file);
	}

	/*
	 * Note:
	 * Check if the symlink is already present.
	 * If the symlink is present, delete it and re-create the symlink.
	 * If the symlink is not present, create the symlink.
	 */
	char link[PATH_MAX];
	memset(link, '\0', PATH_MAX);
	if ((result = sprintf(link, "%s/%s/%s",
				getenv("HOME"), CBIN, pname)) < 0) {
		error("Error while setting up symlink path");
		fprintf(stderr, "Error while setting up symlink path\n");
		return -1;
	}
	debug("Symlink path to be checked: %s", link);
	if (io_path_exists(link)) {
		debug("Symlink already exists, removing and recreating...");
		remove(link);
	} else {
		debug("Sumlink is not present, creating a new one...");
	}

	/*
	 * Note:
	 * sanitize the chosen install location by removing the newline
	 * character at the end of the string
	 */
	if (locations[choice-1][strlen(locations[choice-1])-1] == '\n')
		locations[choice-1][strlen(locations[choice-1])-1] = '\0';
	if (util_symlink(locations[choice-1], link)) {
		error("Error while creating symlink");
		fprintf(stderr, "Error while creating symlink\n");

		for (size_t i = 0; i < lc; i++)
			free(locations[i]);
		if (locations)
			free(locations);

		return -1;
	}

	for (size_t i = 0; i < lc; i++)
		free(locations[i]);
	if (locations)
		free(locations);

	return 0;
}
