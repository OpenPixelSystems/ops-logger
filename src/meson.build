logger_src = [files('logger.c', 'logger-stdio.c')]

if meson.is_cross_build()
  c_args += meson.get_cross_property('cross_args')
  link_args += meson.get_cross_property('cross_link_args')
endif

# loggerlib = shared_library(
#       'logger_shared', # library name
#       logger_src, # source files to be compiled
#       include_directories : inc,
#       c_args : c_args,
#       link_args : link_args,
#       install : false)

