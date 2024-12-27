#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <sys/types.h>

/**
 * @brief Function to create the directories specified in the path.
 *
 * This function can be used to create a single directory as well as to create
 * all the directories provided in the path.
 *
 * @param path - string containing the path to the directory.
 * @param mode - mode type for the directory to be created.
 * @param recursive - boolean, TRUE for recursively creating the directories in
 * the path, FALSE for non-recursive - meaning only create the last directory
 * in the specified path.
 *
 * @return Returns 0 on success, -1 on failure.
 */
int io_mkdir(const char *path, const mode_t mode, bool recursive);

/**
 * @brief Function to resolve the path specified in the source and append the
 * same to the destination.
 *
 * This function can be used to resolve the path from the home directory.
 *
 * @param s - string(const char *) containing the unresolved path.
 *
 * @return Returns if the path is resolved, NULL if resolution failed.
 */
char *io_resolve_path(const char *s);

/**
 * @brief Function to check if the path specified exists or not.
 *
 * This function can be used in order to check if the specified path exists or
 * not.
 *
 * @param path - string(const char *) containing the path to be checked
 * for existence.
 *
 * @return Returns TRUE on success, FALSE on failure.
 */
bool io_path_exists(const char *path);

#endif
