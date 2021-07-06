/**
 * @file logger-stdio.c
 * @brief  Simple stdio driver for logger
 *
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v3.0
 * @date 2020-08-10
 */

#include <stdio.h>
#include <string.h>

#include "logger.h"

char simple_logger_buffer[MAX_STR_LEN + 1] = { 0 };

#define MAX_HDR_LEN 128

static int _printf_wrapper(void *priv, char *str)
{
	(void)priv;

	fprintf(stdout, "%s", str);

	return 0;
}

static const struct logger_ops_t stdio_ops = {
	.init	= NULL,
	.write	= _printf_wrapper,
	.read	= NULL,
	.flush	= NULL,
	.close	= NULL,
};

struct logger_driver_t stdio_logger = {
	.enabled	= true,
	.name		= "stdio",
	.ops		= &stdio_ops,
	.priv_data	= NULL,
};
