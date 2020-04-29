/**
 * @file   logger-wrapper.h
 * @brief  Wrapper for the logger
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @date   20 March 2019
 */
#ifndef _LOGGER_WRAPPER_H_
#define _LOGGER_WRAPPER_H_

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

#ifdef CFG_LOGGER_USE_BUILTIN

/**
 * @brief Log a message with speficied level
 * @param lvl Importance level of the message (see LOG_LVL_* defines)
 * @param msg Message to be logged
 * @param ... Variadic message list
 * @return unused
 */
#define LOG(lvl, msg, ...) \
	logger_log_line(lvl, __FILE__, __FUNCTION__, __LINE__, (msg), ## __VA_ARGS__)

/**
 * @brief Log an info message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_INFO(msg, ...)                                                     \
	logger_log_line(LOG_LVL_INFO, __FILE__, __FUNCTION__, __LINE__, (msg), \
			## __VA_ARGS__)
/**
 * @brief Log a warning message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_WARN(msg, ...)                                              \
	logger_log_line(LOG_LVL_WARN, __FILE__, __FUNCTION__, __LINE__, (msg), \
			## __VA_ARGS__)

/**
 * @brief Log an error message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_ERROR(msg, ...)                                             \
	logger_log_line(LOG_LVL_ERROR, __FILE__, __FUNCTION__, __LINE__,       \
			(msg), ## __VA_ARGS__)

/**
 * @brief Log a debug message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_DEBUG(msg, ...)                                             \
	logger_log_line(LOG_LVL_DEBUG, __FILE__, __FUNCTION__, __LINE__,       \
			(msg), ## __VA_ARGS__)

/**
 * @brief Log a succes message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_OK(msg, ...)                                                \
	logger_log_line(LOG_LVL_OK, __FILE__, __FUNCTION__, __LINE__, (msg),   \
			## __VA_ARGS__)
/**
 * @brief Log a tracing message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_TRACE(msg, ...)                                             \
	logger_log_line(LOG_LVL_TRACING, __FILE__, __FUNCTION__, __LINE__,     \
			(msg), ## __VA_ARGS__)

#else // ifdef CFG_BUILTIN_LOGGER

/**
 * @brief Log an info message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_INFO(msg, ...)                                                     \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

/**
 * @brief Log a warning message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_WARN(msg, ...)                                                     \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

/**
 * @brief Log an error message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_ERROR(msg, ...)                                             \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

/**
 * @brief Log a debug message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_DEBUG(msg, ...)                                                    \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

/**
 * @brief Log a succes message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_OK(msg, ...)                                                       \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

/**
 * @brief Log a tracing message
 *
 * @param msg Message to be logged
 * @param ... Variadic message list
 *
 * @return unused
 */
#define LOG_TRACE(msg, ...)                                                    \
	printf("%s: %s: (%d)" msg "\n", __FILE__, __FUNCTION__, __LINE__,      \
	       ## __VA_ARGS__)

#endif
#endif
