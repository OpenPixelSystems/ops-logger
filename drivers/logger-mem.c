/**
 * @file logger_mem.c
 * @brief  Memory logger
 *
 * This will store logging in memory. Imporant for logging when nor serial or uart is available
 *
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v0.1
 * @date 2020-08-19
 */

#include "logger.h"
#include "memory_map.h"

#define MAX_LOG_LEN 128

struct mem_ctxt_t {
	uint32_t *	mem_start;
	uint32_t *	mem_end;
	uint32_t *	curr_offset;
	uint32_t	count;
};

static void _wipe_old_logging(uint32_t *start, uint32_t *end)
{
	uint32_t *curr = start;

	while (curr != end) {
		*curr = 0x0; //!< wipe data
		curr++;
	}
}

static int _init_mem(void *drv)
{
	struct logger_driver_t *driver = (struct logger_driver_t *)drv;

	if (!driver) {
		return -1;
	}
	struct mem_ctxt_t *ctxt = (struct mem_ctxt_t *)driver->priv_data;

	_wipe_old_logging(ctxt->mem_start, ctxt->mem_end);
	uint32_t size = ctxt->mem_end - ctxt->mem_start;

	LOG_DEBUG("Logger mem size: %d bytes", (size * sizeof(uint32_t)));

	return 0;
}

static int _write_mem(void *drv, struct line_info_t *linfo, char *fmt,
		      va_list *v)
{
	struct logger_driver_t *driver = (struct logger_driver_t *)drv;

	if (!driver) {
		return -1;
	}
	struct mem_ctxt_t *ctxt = (struct mem_ctxt_t *)driver->priv_data;

	*ctxt->curr_offset = ctxt->count;

	char *mem_reg = (char*)ctxt->curr_offset;
	vsnprintf(&mem_reg[4], MAX_LOG_LEN, fmt, *v);

	if (ctxt->curr_offset == ctxt->mem_end)
		ctxt->curr_offset = ctxt->mem_start;
	else
		ctxt->curr_offset += (MAX_LOG_LEN / sizeof(uint32_t));
	ctxt->count++;

	return 0;
}

static struct mem_ctxt_t _ctxt = {
	.mem_start	= (uint32_t *)&__ssram_start__,
	.mem_end	= (uint32_t *)&__ssram_end__,
	.curr_offset	= (uint32_t *)&__ssram_start__,
	.count		= 0,
};

static const struct logger_ops_t mem_ops = {
	.init	= _init_mem,
	.write	= _write_mem,
	.read	= NULL,
	.flush	= NULL,
	.close	= NULL,
};

struct logger_driver_t mem_logger = {
	.enabled	= true,
	.name		= "memory",
	.ops		= &mem_ops,
	.priv_data	= &_ctxt,
};
