#ifndef XVMAN_H
#define XVMAN_H

#include <linux/limits.h>
#include <stdbool.h>

/**
 * @brief xvman configuration structure.
 *
 * This structure will be containing the data of the all the configuration the
 * xvman uses while it is running.
 */
typedef struct {
	char cbin[PATH_MAX]; 		/* custom binary location */
	char confdir[PATH_MAX]; 	/* configuration directory path */
	char conf_fpath[PATH_MAX]; 	/* configuration file path */
	char conf_logfpath[PATH_MAX]; 	/* log file path */
	bool debug; 			/* enable debug mode */
	bool enable_flog; 		/* enable logging to file */
	bool enable_slog; 		/* enable logging to stream */
} xvmanconf_t;

/**
 * @brief Custom binary directory.
 *
 * This custom binary directory will be created in the $HOME directory of the
 * user and the symlinks will be placed here.
 *
 * Note:
 * Once this directory is created, the location should be added to the path by
 * editing the bash configuration file. As of now, only bash will be supported.
 * Later on, will enable support for other shells as well.
 */
#define CBIN ".cbin"

/**
 * @brief Configuration directory path for xvman.
 *
 * All the configurations are to be appended to the $HOME directory.
 */
#define CONFDIR ".config/xvman"

/**
 * @brief Configuration file for xvman.
 *
 * All the program related information will be placed here.
 */
#define CONF_FPATH ".config/xvman/xvmanrc"

/**
 * @brief log file for xvman.
 *
 * This log file will be used in order to log all the details when the program
 * is running.
 */
#define CONF_LOGFPATH ".config/xvman/xvman.log"

/**
 * @brief BASH configuration file location.
 *
 * This is the location of the bash configuration, the data will be added to
 * the user BASH configuration file.
 */
#define BASH_CONF_FILE ".bashrc"

/**
 * @brief Update content for configuration file for default shell.
 *
 * This string is to be used in order to update the configuration file for the
 * defualt shell.
 *
 * @note As of now, it is assumed that the default shell is BASH.
 */
#define RCUPDATE "PATH=$PATH:$HOME/.cbin"

/**
 * @brief Lock file path.
 *
 * This is the location where the lockfile will be created post updating the
 * shell configuration file for the first time.
 *
 * @note As of now, this program will be updating only the BASH configuration
 * file.
 */
#define LOCKFILE ".config/xvman/.lockfile"

/**
 * @brief Show the program usage.
 *
 * This function can be used in order to show the usage and the options
 * available for this program(xvman - linux/unix user level version manager).
 *
 * @author oldgod
 * @date Sat, 07 Dec 2024 08:43:07 +0530
 */
void xvman_show_usage(void);

/**
 * @brief Free all the memory that is being used by this program.
 *
 * This function can be used to free up all the memory being allocated by this
 * program and used when the program is running.
 */
void xvman_free_mem(void);

/**
 * @brief Setup the pre-requisites.
 *
 * This function needs to be called first in order for pre-requisites to be set
 * for xvman. Otherwise the program might show peculiar behaviour.
 *
 * @param config - pointer to the config structure for xvman.
 * @return Returns 0 on success, -1 on failure.
 */
int xvman_setup_prereq(xvmanconf_t *config);

/**
 * @brief Function to add a program and associated install location.
 *
 * This function can be called in order to set up a program and associated
 * install location into xvman.
 *
 * @param data - string containing the name of hte program as well as the
 * install ocation.
 *
 * @param data - string containing the name of the program as well as the
 * install location. The string is separated by a space, the first value is the
 * name of the program, the second is the install location.
 *
 * @return returns 0 on success, -1 on failure.
 */
int xvman_add(const char *data);

/**
 * @brief Function to configure the version to be made as default.
 *
 * This function needs to be called when the user wants to choose an already
 * added install location as the default version.
 *
 * @param pname - string containing the name of the already configured program.
 *
 * @return Returns -1 on failure, 0 on success.
 */
int xvman_config(const char *pname);

#endif
