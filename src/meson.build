loggersrc = [files('logger.c', 'queue.c')]
thread_dep = dependency('threads')

loggerlib = shared_library(
    	'loggerlib', # library name
	loggersrc, # source files to be compiled
    	include_directories : inc,
	dependencies : thread_dep,
        install : false)
