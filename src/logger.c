/**
 * @file logger-v3.c
 * @brief  Logger V3
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v3.0
 * @date 2020-08-10
 */

#include "logger.h"

#if defined(CFG_LOGGER_SIMPLE_LOGGER) && !defined(CFG_LOGGER_ADV_LOGGER)
extern struct logger_driver_t stdio_logger;
#endif

static struct logger_driver_t *active_drivers[] = {
#if defined(CFG_LOGGER_SIMPLE_LOGGER) && !defined(CFG_LOGGER_ADV_LOGGER)
	&stdio_logger,
#endif
	NULL,
};

struct log_level_t _log_levels[] = {
	{ LOG_LVL_DEBUG, "DEBUG",   MAGENTA, 0	   },
	{ LOG_LVL_INFO,	 "INFO",    BLUE,    0	   },
	{ LOG_LVL_OK,	 "OKAY",    GREEN,   0	   },
	{ LOG_LVL_WARN,	 "WARN",    YELLOW,  0	   },
	{ LOG_LVL_ERROR, "ERROR",   RED,     0	   },
};

static int _current_loglvl = LOG_LVL_EXTRA;

/**
 * @brief  Convert a LOG mask to an ID in the _log_levels struct
 * @param mask The mask to be converted
 * @returns  A valid id with then _log_levels struct
 */
int logger_mask2id(int mask)
{
	int n = 1;
	int i = 0;

	while (!(mask & n)) {
		n = n << 1;
		i++;
	}
	return i;
}

void logger_log(const int lvl, const char *file, const char *fn, const int ln,
		char *fmt, ...)
{
	if (!(lvl & _current_loglvl)) {
		return;
	}

	va_list va;

	va_start(va, fmt);
	struct line_info_t linfo = {
		.lvl	= lvl,
		.file	= file,
		.fn	= fn,
		.ln	= ln,
	};

	for (int i = 0; active_drivers[i] != NULL; i++) {
		active_drivers[i]->ops->write(0, &linfo, fmt, &va);
	}

	va_end(va);
}
