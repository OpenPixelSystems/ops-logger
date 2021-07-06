/**
 * @file logger-stm32-uart.c
 * @brief  UART driver for logger
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v0.1
 * @date 2020-12-08
 */

#include <stdio.h>
#include <string.h>

#include "usart.h"
#include "main.h"

#include "logger.h"

static uint8_t uart_logger_buffer[MAX_STR_LEN + 1] = { 0 };

#define MAX_HDR_LEN 128

struct uart_logger_ctxt_t {
	UART_HandleTypeDef *handle;
};

struct uart_logger_ctxt_t _ctxt = {
#if defined(STM32H750_DEVKIT)
	.handle = &huart3,
#elif defined(ETF0009)
	.handle = &huart4,
#else
#error "Invalid board config"
#endif
};

void memdump(uint8_t *data, size_t len)
{
	for (size_t i = 0, j = 1; i < len; i++, j++) {
		uint8_t uart_buffer[10] = { 0 };
		snprintf((char *)uart_buffer, 10, "0x%.2x ", data[i]);
		HAL_UART_Transmit(_ctxt.handle, uart_buffer, 10, 10000);
		if (j == 20) {
			LOG_RAW(" ");
			j = 0;
		}
	}
	LOG_RAW(" ");
}

static int _init_uart(void *drv)
{
	struct logger_driver_t *driver = (struct logger_driver_t *)drv;
	if (!driver) {
		return -1;
	}
	driver->priv_data = &_ctxt;

	return 0;
}

static void _print_header(struct line_info_t *linfo)
{
	memset(uart_logger_buffer, 0, MAX_STR_LEN + 1);
	snprintf((char *)uart_logger_buffer, MAX_HDR_LEN,
		 "[%s%5s%s] (%20s)(%30s @%3d) : ",
		 _log_levels[logger_mask2id(linfo->lvl)].color,
		 _log_levels[logger_mask2id(linfo->lvl)].name,
		 RESET, linfo->file, linfo->fn, linfo->ln);
	HAL_UART_Transmit(_ctxt.handle, uart_logger_buffer,
			  strlen((char *)uart_logger_buffer), 1000);
}

static int _write_uart(void *priv, struct line_info_t *linfo, char *fmt,
		       va_list *v)
{
	(void)priv;

	if (linfo->lvl != LOG_LVL_RAW) {
		_print_header(linfo);
	}

	memset(uart_logger_buffer, 0, MAX_STR_LEN + 1);
	vsnprintf((char *)uart_logger_buffer, MAX_STR_LEN, fmt, *v);
	HAL_UART_Transmit(_ctxt.handle, uart_logger_buffer,
			  strlen((char *)uart_logger_buffer), 1000);
	HAL_UART_Transmit(_ctxt.handle, (uint8_t *)"\r\n", 2, 1000);
	return 0;
}

static const struct logger_ops_t uart_ops = {
	.init	= _init_uart,
	.write	= _write_uart,
	.read	= NULL,
	.flush	= NULL,
	.close	= NULL,
};

struct logger_driver_t uart_logger = {
	.enabled	= true,
	.name		= "uart",
	.ops		= &uart_ops,
	.priv_data	= NULL,
};
