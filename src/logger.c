/**
 * @file util-logger.c
 * @brief  Simple logger implementation
 * @author Bram Vlerick <bram.vlerick@openpixelsystems.org>
 * @version v2.0
 * @date 2020-04-29
 */

#include "logger.h"
#include "logger-wrapper.h"


static long _log_counter = 0;
static int _current_loglvl = LOG_LVL_EXTRA;

#ifndef CFG_LOGGER_DEEP_EMBEDDED

#include "queue.h"

static bool _is_threaded = false;
static bool _thread_started = false;

pthread_mutex_t _rotated_lock = PTHREAD_MUTEX_INITIALIZER;

static bool _logfile_enabled = false;

static pthread_t _logger_thread;
static struct  queue_t *_logger_queue;

static FILE *_logfile = NULL;
static char _logfile_name[LOGGER_MAX_LOGFILE_NAME];

#ifdef CFG_LOGGER_SPLIT_ERROR_LOGS
static FILE *_errorfile = NULL;
#endif /* CFG_LOGGER_SPLIT_ERROR_LOGS */

#endif /* CFG_LOGGER_DEEP_EMBEDDED */

static char *_current_filter;

struct log_message_t {
	int	log_lvl_id;
	int	log_lvl_mask;
	char	thread[LOGGER_MAX_THREAD_NAME];
	char *	prefix;
	char *	msg;
};

static struct log_level_t _log_levels[] = {
	{ LOG_LVL_INFO,	   "INFO",  BLUE,    0 },
	{ LOG_LVL_WARN,	   "WARN",  YELLOW,  0 },
	{ LOG_LVL_ERROR,   "ERROR", RED,     0 },
	{ LOG_LVL_DEBUG,   "DEBUG", MAGENTA, 0 },
	{ LOG_LVL_OK,	   "OKAY",  GREEN,   0 },
	{ LOG_LVL_TRACING, "TRACE", CYAN,    0 },
};

/**
 * @brief  Convert a LOG mask to an ID in the _log_levels struct
 * @param mask The mask to be converted
 * @returns  A valid id with then _log_levels struct
 */
static int _mask2id(int mask)
{
	int n = 1;
	int i = 0;

	while (!(mask & n)) {
		n = n << 1;
		i++;
	}
	return i;
}

static int _build_msg_prefix(struct log_message_t *msg, const char *file, const char *function, const unsigned int line,
			     bool for_file)
{
	if (!msg) {
		return -1;
	}

	msg->prefix = malloc(sizeof(char) * LOGGER_MAX_PREFIX_LEN);
	if (!msg->prefix) {
		return -1;
	}

#ifndef CFG_LOGGER_DEEP_EMBEDDED
	pthread_getname_np(pthread_self(), msg->thread, LOGGER_MAX_THREAD_NAME);
	if (for_file) {
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

		snprintf(msg->prefix, LOGGER_MAX_PREFIX_LEN,
			 "[%d-%.2d-%.2d %.2d:%.2d:%.2d][%10.10s][%5.5s][%.25s: %.30s: %4u]: ",
			 tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec,
			 msg->thread, _log_levels[msg->log_lvl_id].name, file, function, line);
	} else {
#endif /* CFG_LOGGER_DEEP_EMBEDDED */
		snprintf(msg->prefix, LOGGER_MAX_PREFIX_LEN,
			 "[%10.10s][%s%5.5s%s][%15.15s: %30.30s: %4u]: ",
			 msg->thread, _log_levels[msg->log_lvl_id].color,
			 _log_levels[msg->log_lvl_id].name, RESET, file, function, line);
#ifndef CFG_LOGGER_DEEP_EMBEDDED
	}
#endif /* CFG_LOGGER_DEEP_EMBEDDED */
	return 0;
}

static int _build_msg_string(struct log_message_t *msg, const char *format, va_list va)
{
	if (!msg) {
		return -1;
	}
	msg->msg = malloc(sizeof(char) * LOGGER_MAX_MSG_LEN);
	if (!msg->msg) {
		return -1;
	}

	vsnprintf(msg->msg, LOGGER_MAX_MSG_LEN, format, va);

	return 0;
}

static bool _check_msg_for_filter(struct log_message_t *msg)
{
	/* A filter is enabled */
	if (_current_filter) {
		if (strstr(msg->msg, _current_filter) == NULL && strstr(msg->prefix, _current_filter) == NULL) {
			return false;
		}
		return true;
	}
	return true;
}

void _cleanup_log_msg(struct log_message_t *msg)
{
	if (msg) {
		if (msg->prefix) {
			free(msg->prefix);
		}
		if (msg->msg) {
			free(msg->msg);
		}
		free(msg);
	}
}

void _logger_print_and_free_msg(struct log_message_t *msg)
{
	fprintf(stdout, "%s", msg->prefix);
	fprintf(stdout, "%s\n", msg->msg);
	_cleanup_log_msg(msg);
}

#ifndef CFG_LOGGER_DEEP_EMBEDDED
void _logger_flush_queue()
{
	if (!_logger_queue) {
		return;
	}

	for (;;) {
		struct log_message_t *msg = NULL;
		int error = queue_pop(_logger_queue, (void **)&msg);
		if (error < 0) {
			break;
		}
		_logger_print_and_free_msg(msg);
	}
}

#define B_TO_MB (1024 * 1000)
int _check_and_perform_log_rotate(FILE *fp, bool is_errorlog)
{
	if (!fp || !_logfile_enabled) {
		return -1;
	}

	struct stat st;
	fstat(fileno(fp), &st);
	int size = st.st_size / B_TO_MB;

	if (size > CFG_LOGGER_MAX_LOG_SIZE) {
		logger_disable_file_logging();

		LOG_INFO("Performing LOG Rotate on %s!", _logfile_name);

		char current_file[LOGGER_MAX_LOGFILE_NAME + 4 + 1];     /* +4 for .log, + 1 \0 */
		char backup_file[LOGGER_MAX_LOGFILE_NAME + 4 + 4 + 1];  /*+4 for .old +4 .err + 1 \0 */
		if (is_errorlog) {
			snprintf(current_file, LOGGER_MAX_LOGFILE_NAME + 4, "%s.err", _logfile_name);
			snprintf(backup_file, LOGGER_MAX_LOGFILE_NAME + 4 + 4, "%s.err.old", _logfile_name);
		} else {
			snprintf(current_file, LOGGER_MAX_LOGFILE_NAME + 4, "%s.log", _logfile_name);
			snprintf(backup_file, LOGGER_MAX_LOGFILE_NAME + 4, "%s.old", _logfile_name);
		}

		int error = 0;

		pthread_mutex_lock(&_rotated_lock);

		if (lstat(backup_file, &st) == 0) {
			error = unlink(backup_file);
			if (error < 0) {
				LOG_ERROR("Failed to remove old backup file");
				pthread_mutex_unlock(&_rotated_lock);
				return -1;
			}
		}

		error = rename(current_file, backup_file);
		if (error < 0) {
			LOG_ERROR("Failed to move backup file!");
			pthread_mutex_unlock(&_rotated_lock);
			return -1;
		}

		logger_enable_file_logging(_logfile_name);
		pthread_mutex_unlock(&_rotated_lock);

		return 1;
	}
	return 0;
}

void *_logger_internal_thread(void *data)
{
	LOG_INFO("Running logger thread");

	_thread_started = true;

	while (_thread_started) {
		struct log_message_t *msg;
		int error = queue_pop(_logger_queue, (void **)&msg);
		if (error < 0) {
			usleep(100);
			continue;
		}
		_logger_print_and_free_msg(msg);
	}
	_logger_flush_queue();
	return NULL;
}
#endif /* CFG_LOGGER_DEEP_EMBEDDED */

void logger_log_line(int log_lvl_mask, char *file, const char *function, const unsigned int line, char *format, ...)
{
	struct log_message_t *msg = NULL;
	va_list va;

	if (!(log_lvl_mask & _current_loglvl)) {
		goto error;
	}

	msg = malloc(sizeof(struct log_message_t));
	if (!msg) {
		goto error;
	}

	msg->log_lvl_id = _mask2id(log_lvl_mask);
	msg->log_lvl_mask = log_lvl_mask;

	char *canon_file = basename(file);
	if (!canon_file) {
		free(msg);
		goto error;
	}


	int error = _build_msg_prefix(msg, canon_file, function, line, false);
	if (error < 0) {
		goto error;
	}

	va_start(va, format);
	error = _build_msg_string(msg, format, va);
	if (error < 0) {
		goto error;
	}
	va_end(va);

	if (_check_msg_for_filter(msg)) {
#ifndef CFG_LOGGER_DEEP_EMBEDDED
		if (_is_threaded) {
			error = queue_push(_logger_queue, (void *)msg);
			if (error < 0) {
				goto error;
			}
		} else {
#endif
			_logger_print_and_free_msg(msg);
#ifndef CFG_LOGGER_DEEP_EMBEDDED
		}
#endif
	} else {
		_cleanup_log_msg(msg);
	}

#ifndef CFG_LOGGER_DEEP_EMBEDDED
	if (_logfile_enabled) {
		error = _check_and_perform_log_rotate(_logfile, false);
		if (error < 0) {
			LOG_ERROR("Log rotate failed!");
			goto file_end;
		}

#ifdef CFG_LOGGER_SPLIT_ERROR_LOGS
		_check_and_perform_log_rotate(_errorfile, true);
		if (error < 0) {
			LOG_ERROR("Error log rotate failed!");
			goto file_end;
		}
#endif /*CFG_LOGGER_SPLIT_ERROR_LOGS */

		struct log_message_t file_msg = { 0 };

		file_msg.log_lvl_id = _mask2id(log_lvl_mask);
		file_msg.log_lvl_mask = log_lvl_mask;

		error = _build_msg_prefix(&file_msg, canon_file, function, line, true);
		if (error < 0) {
			LOG_ERROR("Failed to build file prefix");
			goto file_end;
		}

		va_start(va, format);
		error = _build_msg_string(&file_msg, format, va);
		if (error < 0) {
			LOG_ERROR("Failed to build file msg");
			goto file_end;
		}
		va_end(va);

		pthread_mutex_lock(&_rotated_lock);
		if (_logfile) {
			fprintf(_logfile, "%ld - ", _log_counter);
			fprintf(_logfile, "%s", file_msg.prefix);
			fprintf(_logfile, "%s\n", file_msg.msg);
		}
#ifdef CFG_LOGGER_SPLIT_ERROR_LOGS
		if ((file_msg.log_lvl_mask & LOG_LVL_WARN) || file_msg.log_lvl_mask & LOG_LVL_ERROR) {
			if (_errorfile) {
				fprintf(_errorfile, "%ld", _log_counter);
				fprintf(_errorfile, "%s", file_msg.prefix);
				fprintf(_errorfile, "%s\n", file_msg.msg);
			}
		}
#endif /* CFG_LOGGER_SPLIT_ERROR_LOGS */
		pthread_mutex_unlock(&_rotated_lock);
file_end:
		if (file_msg.prefix) {
			free(file_msg.prefix);
		}
		if (file_msg.msg) {
			free(file_msg.msg);
		}
		_log_counter++;
	}
#endif /* CFG_LOGGER_DEEP_EMBEDDED */
	va_end(va);
	_log_levels[_mask2id(log_lvl_mask)].counter++;
	return;

error:
	va_end(va);
	_cleanup_log_msg(msg);
}

#ifndef CFG_LOGGER_DEEP_EMBEDDED

int logger_enable_file_logging(const char *filename)
{
	strncpy(_logfile_name, filename, LOGGER_MAX_LOGFILE_NAME);

	char tmp_name[LOGGER_MAX_LOGFILE_NAME + 4]; // +4 for .log
	snprintf(tmp_name, LOGGER_MAX_LOGFILE_NAME + 4, "%s.log", _logfile_name);
	_logfile = fopen(tmp_name, "a+");
	if (!_logfile) {
		LOG_ERROR("Enable file logging failed\n");
		return -1;
	}

#ifdef CFG_LOGGER_SPLIT_ERROR_LOGS
	snprintf(tmp_name, LOGGER_MAX_LOGFILE_NAME + 4, "%s.err", _logfile_name);
	_errorfile = fopen(tmp_name, "a+");
	if (!_logfile) {
		LOG_ERROR("Enable file logging failed\n");
		return -1;
	}
#endif /* CFG_LOGGER_SPLIT_ERRORS */

	_logfile_enabled = true;
	return 0;
}

void logger_disable_file_logging()
{
	_logfile_enabled = false;
	fclose(_logfile);
	_logfile = NULL;

#ifdef CFG_LOGGER_SPLIT_ERROR_LOGS
	fclose(_errorfile);
	_errorfile = NULL;
#endif /* CFG_LOGGER_SPLIT_ERROR_LOGS */
}

int logger_get_nb_message_loglevel(int loglvl)
{
	return _log_levels[_mask2id(loglvl)].counter;
}

int logger_enable_threaded_mode()
{
	_is_threaded = true;
	_logger_queue = queue_create(free);
	if (!_logger_queue) {
		LOG_ERROR("Failed to create queue");
		return -1;
	}

	int error = pthread_create(&_logger_thread, NULL, _logger_internal_thread, NULL);
	if (error < 0) {
		LOG_ERROR("Failed to create thread");
		return -1;
	}

	while (!_thread_started) {
		usleep(100);
	}

	return 0;
}

void logger_disable_threaded_mode()
{
	if (!_thread_started) {
		return;
	}
	_is_threaded = false;
	_thread_started = false;
	pthread_join(_logger_thread, NULL);
	if (_logger_queue) {
		queue_destroy(_logger_queue);
	}
	LOG_INFO("Ended logger thread");
}
#endif /*CFG_LOGGER_DEEP_EMBEDDED */

#define MAX_FILTER_LEN 50 //!< Arbitrary filter length
int logger_enable_log_filter(char *filter)
{
	if (!filter) {
		logger_disable_log_filter();
		return 0;
	}

	int new_filter_len = strlen(filter);
	/* Limit filter to *some* length (performance wise) */
	if (new_filter_len > MAX_FILTER_LEN) {
		LOG_ERROR("Filter to big!");
		return -1;
	}

	if (_current_filter) {
		/* Clean old filter */
		free(_current_filter);
		_current_filter = NULL;
	}

	if (!strcmp(filter, "")) {
		/* New filter is empty */
		logger_disable_log_filter();
	}

	_current_filter = malloc(new_filter_len + 1);
	if (!_current_filter) {
		_current_filter = NULL;
		return -1;
	}

	memset(_current_filter, 0, new_filter_len + 1);
	strncpy(_current_filter, filter, new_filter_len);

	LOG_OK("Filter '%s' enabled!", _current_filter);
	return 0;
}

void logger_disable_log_filter()
{
	if (_current_filter) {
		free(_current_filter);
	}
	_current_filter = NULL;
}

void logger_set_loglevel(int loglvl)
{
	_current_loglvl = loglvl;
}

int logger_init()
{
#ifndef CFG_LOGGER_DEEP_EMBEDDED
	int error = logger_enable_threaded_mode();

	if (error < 0) {
		return -1;
	}

#ifdef CFG_LOGGER_DEFAULT_LOGFILE_ENABLED
	error = logger_enable_file_logging("./system-log");
	if (error < 0) {
		return -1;
	}
#endif /* CFG_LOGGER_DEFAULT_LOGFILE_ENABLED */
#endif /* CFG_LOGGER_DEEP_EMBEDDED */

	return 0;
}

void logger_exit()
{
#ifndef CFG_LOGGER_DEEP_EMBEDDED
	logger_disable_threaded_mode();
	logger_disable_file_logging();
#endif
	logger_disable_log_filter();
}

void logger_print_stats(void)
{
	int i = 0;
	int nr_loglvls = sizeof(_log_levels) / sizeof(struct log_level_t);

	/* LOG_INFO("Nr of loglevels: %d", nr_loglvls); */
	for (i = 0; i < nr_loglvls; i++) {
		printf("%d: %s%10s%s: %d entries\n", _mask2id(
			       _log_levels[i].mask), _log_levels[i].color, _log_levels[i].name, RESET,
		       _log_levels[i].counter);
	}
}
