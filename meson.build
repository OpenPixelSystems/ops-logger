# OPS Logger Lib

project('logger', 'c', version : '3.0.1')

c_args = []
link_args = []

if meson.is_subproject()
  c_args += '-DCFG_LOGGER_EXTERNAL_DRIVER_CONF'
endif

if meson.is_cross_build()
  c_args += meson.get_cross_property('cross_args')
  link_args += meson.get_cross_property('cross_link_args')
endif

c_args += get_option('buildtype') == 'release' ? ['-DNDEBUG'] : ['-DDEBUG=1', '-g', '-ggdb']
c_args += '-DCFG_RING_ENABLED'

logger_includes = include_directories(['./include'])
logger_srcs = files(['./src/logger.c', './src/logger-stdio.c'], './src/cbuffer.c')

if not meson.is_cross_build()
  subdir('test')
endif
