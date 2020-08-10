/**
 * @file logger-v3.h
 * @brief Main include file for logger
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v3.0
 * @date 2020-08-10
 */

#ifndef _LOGGER_V3_H_
#define _LOGGER_V3_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

#include "colors.h"

#define LOG_LVL_DEBUG           0x00000001      //!< Debugging
#define LOG_LVL_INFO            0x00000002      //!< Info
#define LOG_LVL_OK              0x00000004      //!< Success
#define LOG_LVL_WARN            0x00000008      //!< Warning
#define LOG_LVL_ERROR           0x00000010      //!< Error

/** All standard Logging */
#define LOG_LVL_ALL (LOG_LVL_INFO | LOG_LVL_WARN | LOG_LVL_ERROR | LOG_LVL_OK)

/** Acceptable level of logging for production software */
#define LOG_LVL_PRODUCTION (LOG_LVL_OK | LOG_LVL_WARN | LOG_LVL_ERROR)

/** Extra debugging information */
#define LOG_LVL_EXTRA (LOG_LVL_ALL | LOG_LVL_DEBUG)

/** No logging at all */
#define LOG_LVL_NONE 0


#define LOGGER_DRV_NAME 16
#define MAX_STR_LEN 256

struct line_info_t {
	const int lvl;
	const char *file;
	const char *fn;
	const int ln;
};

typedef int (*write_fn)(void *priv, struct line_info_t *linfo, char *fmt, va_list *v);
typedef int (*read_fn)(int fd, char *buffer);
typedef int (*flush_fn)(int fd);

struct logger_ops_t {
	write_fn write;
	read_fn read;
	flush_fn flush;
};

struct logger_driver_t {
	char name[LOGGER_DRV_NAME];
	const struct logger_ops_t *ops;
	void *priv_data;
};

/**
 * @brief  Log level definition
 */
struct log_level_t {
	int		mask;           //!< Mask associated with the log level
	const char *	name;           //!< Log level name. (What will be printed before msg)
	const char *	color;          //!< Color for the name
	int		counter;        //!< Internal counter for number of messages
};

extern struct log_level_t _log_levels[];

int logger_mask2id(int loglvl);

void logger_log(const int lvl, const char *file, const char *fn, const int ln, char *fmt, ...);

#define LOG_DEBUG(msg, ...) \
	logger_log(LOG_LVL_DEBUG, __FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

#define LOG_INFO(msg, ...) \
	logger_log(LOG_LVL_INFO, __FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

#define LOG_OK(msg, ...) \
	logger_log(LOG_LVL_OK, __FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

#define LOG_WARN(msg, ...) \
	logger_log(LOG_LVL_WARN, __FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

#define LOG_ERROR(msg, ...) \
	logger_log(LOG_LVL_ERROR, __FILE__, __FUNCTION__, __LINE__, msg, ## __VA_ARGS__)

#endif /* _LOGGER_V3_H_ */
