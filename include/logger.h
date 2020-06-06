/**
 * @file util/logger.h
 * @brief  Simple logger
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v2.0
 * @date 2019-06-03
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#ifndef CFG_LOGGER_DEEP_EMBEDDED
#include <pthread.h>
#include <sys/stat.h>

#include <libgen.h>             /* basename() */
#endif

#include "colors.h"

#define LOG_LVL_INFO            0x00000001      //!< Info
#define LOG_LVL_WARN            0x00000002      //!< Warning
#define LOG_LVL_ERROR           0x00000004      //!< Error
#define LOG_LVL_DEBUG           0x00000008      //!< Debugging
#define LOG_LVL_OK              0x00000010      //!< Success
#define LOG_LVL_TRACING         0x00000020      //!< Tracing

/** All standard Logging */
#define LOG_LVL_ALL (LOG_LVL_INFO | LOG_LVL_WARN | LOG_LVL_ERROR | LOG_LVL_OK)

/** Acceptable level of logging for production software */
#define LOG_LVL_PRODUCTION (LOG_LVL_OK | LOG_LVL_WARN | LOG_LVL_ERROR)

/** Extra debugging information */
#define LOG_LVL_EXTRA (LOG_LVL_ALL | LOG_LVL_DEBUG | LOG_LVL_TRACING)

/** No logging at all! */
#define LOG_LVL_NONE 0

#define LOGGER_MAX_THREAD_NAME 16
#define LOGGER_MAX_LOGFILE_NAME 64
#define LOGGER_MAX_PREFIX_LEN 128

#ifndef CFG_LOGGER_MAX_LOG_SIZE
#define CFG_LOGGER_MAX_LOG_SIZE 20 /* MB */
#endif

#ifdef CFG_LOGGER_NO_MALLOC
#define LOGGER_MAX_MSG_LEN 128
#else
#define LOGGER_MAX_MSG_LEN 2048
#endif /* CFG_LOGGER_NO_MALLOC */

/**
 * @brief  Log level definition
 */
struct log_level_t {
	int		mask;           //!< Mask associated with the log level
	const char *	name;           //!< Log level name. (What will be printed before msg)
	const char *	color;          //!< Color for the name
	int		counter;        //!< Internal counter for number of messages
};

/**
 * @brief Printf style loggin function
 * @param log_lvl Logging level
 * @param file File name
 * @param function Function name
 * @param line Line number
 * @param format Printf style formating
 * @param ... VA list for format arguments
 */
void logger_log_line(int log_lvl, char *file, const char *function, const unsigned int line, char *format, ...);

/**
 * @brief  Enable file logging
 * @param filename File to be used for logging
 * @returns  -1 if failed otherwise 0
 */
int logger_enable_file_logging(const char *filename);

/**
 * @brief  Disable file logging
 */
void logger_disable_file_logging();

/**
 * @brief Enable/Disable threaded logging
 * @returns -1 if failed , 0 if successful
 */
int logger_enable_threaded_mode();

/**
 * @brief Enable/Disable threaded logging
 * @returns -1 if failed , 0 if successful
 */
void logger_disable_threaded_mode();

/**
 * @brief Enable a filter on the logging
 * @param filter Filter string
 * @note An empty string (\"\") will clear the filter
 * @returns -1 if failed , 0 if successful
 */
int logger_enable_log_filter(char *filter);

/**
 * @brief Clear the filter of the logger
 */
void logger_disable_log_filter();

/**
 * @brief Set logging level
 * Set the current loglevel of the program
 * @param loglvl Level that will be set
 */
void logger_set_loglevel(int loglvl);

/**
 * @brief Initialize the logger
 * Function mainly used to initialize threaded mode
 * (If compiled with threaded support)
 */
int logger_init();

/**
 * @brief Perform proper cleanup op logger
 */
void logger_exit();

/**
 * @brief Print the logger statistics
 */
void logger_print_stats(void);

/**
 * @brief Get number of message for a specific level
 * @param loglvl Level that will be set
 */
int logger_get_nb_message_loglevel(int loglvl);

#endif
