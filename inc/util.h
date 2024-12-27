#ifndef UTIL_H
#define UTIL_H

/**
 * @brief Utility function to create a symlink.
 *
 * This function can be used in order to create a symlink of the specified
 * target resource to the link destination.
 *
 * @param target - string contains the path of the target resource.
 * @param link - string containing the destination path of the symlink.
 *
 * @return Returns -1 on failure, 0 on success.
 */
int util_symlink(const char *target, const char *link);

#endif
