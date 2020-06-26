loggersrc = [files('logger.c', 'queue.c')]

deps = []
if meson.is_cross_build() == false
deps += dependency('threads')
endif


if host_machine.system() == 'none'
     loggerlib = static_library(
	    'loggerlib', # library name
	    loggersrc, # source files to be compiled
	    include_directories : inc,
	    dependencies : deps,
	    c_args : c_args,
	    link_args : link_args,
	    install : false)
else
    loggerlib = shared_library(
	    'loggerlib', # library name
	    loggersrc, # source files to be compiled
	    include_directories : inc,
	    dependencies : deps,
	    c_args : c_args,
	    link_args : link_args,
	    install : false)
endif
