# loggersrc = [files('logger.c')]

logger_src = [files('logger.c', 'logger-stdio.c')]

# deps = []
# if meson.is_cross_build() == false
#   loggersrc += [files('queue.c')]
#   deps += dependency('threads')
# endif


# if host_machine.system() == 'none'
#      loggerlib = static_library(
# 	    'loggerlib', # library name
# 	    loggersrc, # source files to be compiled
# 	    include_directories : inc,
# 	    dependencies : deps,
# 	    c_args : c_args,
# 	    link_args : link_args,
# 	    install : false)
# else
#     loggerlib = shared_library(
# 	    'loggerlib', # library name
# 	    loggersrc, # source files to be compiled
# 	    include_directories : inc,
# 	    dependencies : deps,
# 	    c_args : c_args,
# 	    link_args : link_args,
# 	    install : false)
# endif

loggerlib = shared_library(
      'loggerlib', # library name
      logger_src, # source files to be compiled
      include_directories : inc,
      # dependencies : deps,
      c_args : c_args,
      link_args : link_args,
      install : false)

