/**
 * @file log.h
 * @copyright (c) 2018 Sayantan, Nilangshu. All rights reserved.
 * @brief Program logging handling module
 * @details This module will be used for logging as well as debugging. The
 * user(s) of the program can also debug by looking at the log files.
 */

#ifndef LOG_H
#define LOG_H

#include <stdbool.h>
#include <stdarg.h>

/**
 * @brief The logging string for variable arguments
 * @details This is the logging string which will be used while writing to the
 * log file. The format will be having the following information:
 * log_level: __TIME_STAMP__ __FILE__:__FUNCTION__[__LINE__]
 */
#define LOG_STRF "%s %s%s:%s()[%ld]"

/**
 * @brief The string format for basic string logging
 * @details This is the logging string which will be used while writing to the
 * log file. The format will be having the following information:
 * log_level: __FILE__:__FUNCTION__[__LINE__] msg
 */
#define LOG_STR "%s%s:%s()[%ld] %s\n"

/**
 * @brief Default name of the log file if one is not specified
 */
#define DEFAULT_LOG_FILE "./default.log"

/**
 * @brief enum containing the log levels - DEBUG, WARN, ERROR, INFO
 */
enum log_level {
        DEBUG = 0x01,
        WARN,
        ERROR,
        INFO
};

/**
 * @brief enum containing the init states - FALSE(0) and TRUE
 */
enum init_state {
	FALSE = 0,
	TRUE
};

/**
 * @brief shortened version of the DEBUG level log_write_fmt function
 */
#define debug(x, ...) log_write_fmt(x, __FILE__, __FUNCTION__, __LINE__,\
                DEBUG, ##__VA_ARGS__)

/**
 * @brief shortened version of the WARN level log_write_fmt function
 */
#define warning(x, ...) log_write_fmt(x, __FILE__, __FUNCTION__, __LINE__,\
                WARN, ##__VA_ARGS__)

/**
 * @brief shortened version of the ERROR level log_write_fmt function
 */
#define error(x, ...) log_write_fmt(x, __FILE__, __FUNCTION__, __LINE__,\
                ERROR, ##__VA_ARGS__)

/**
 * @brief shortened version of the INFO level log_write_fmt function
 */
#define info(x, ...) log_write_fmt(x, __FILE__, __FUNCTION__, __LINE__,\
                INFO, ##__VA_ARGS__)

/**
 * @brief default log file name provided while initializing the module
 */
#define init(ll) log_init(DEFAULT_LOG_FILE, ll)

/**
 * @brief the logger module initiator
 * @param[in] f NULL will cause the function to return the control to the
 * calling function and stop the final appending of lines
 * @param[in] ll refers to the LOG_LEVEL. This needs to be filled out. Without
 * providing the log level the logger would not work out
 * @note This function has to be called out if the logging needs to be done
 */
void log_init(const char *f, enum log_level ll);

/**
 * @brief the logger module stream setter
 * @param[in] ostream boolean - set to enable stdout streaming. By default, if
 * this function is not called, logging to stdout is enabled.
 * @param[in] fstream boolean - set to enable file streaming. By default, if
 * this function is not called, logging to stdout is disabled.
 * @note This function has to be called out if the logging needs to be done
 */
void log_set_stream(bool ostream, bool fstream);

/**
 * @brief Variadic function for logging specific string format
 * @param[in] s NULL string will return the control, but else the null
 * terminated string will be written in the log file.
 * @param[in] ll ENUM for log_level - DEBUG/WARN/ERROR/INFO
 * @param[in] fi File information from which function has been called
 * @param[in] fu Function information from which this logging function is being
 * called
 * @param[in] ln Line number from where the logging function was invoked
 * @details This function will be responsible for writing into the log file the
 * message that has been passed to it
 */
void log_write_fmt(const char *fmt, const char *fi, const char *fu, long ln,
                int ll, ...);

/**
 * @brief free the memory allocated for the log file name
 * @details This function needs to be called in order to free the memory
 * allocated to the log file name pointer.
 * @note This must be called before exiting the program, else this will result
 * in memory leak.
 */
void log_free_lf(void);

#endif

