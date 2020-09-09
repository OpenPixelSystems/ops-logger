project('logger','c')
inc = include_directories('include')

add_project_arguments(['-DCFG_LOGGER_SIMPLE_LOGGER'], language: 'c')

c_args = []
link_args = []

if meson.is_cross_build()
  c_args += meson.get_cross_property('cross_args')
  link_args += meson.get_cross_property('cross_link_args')
endif

c_args += get_option('buildtype') == 'release' ? '-DNDEBUG' : '-DDEBUG=1 -g -ggdb'

subdir('./src')

liblogger = static_library('logger', logger_src, include_directories : inc, c_args : c_args,
  link_args : link_args, install : false)

logger_dep = declare_dependency(include_directories: inc, link_with: liblogger)
