# loggersrc = [files('logger.c')]

loggerv3_src = [files('logger-v3.c', 'logger-v3-stdio.c')]

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
      loggerv3_src, # source files to be compiled
      include_directories : inc,
      # dependencies : deps,
      c_args : c_args,
      link_args : link_args,
      install : false)

