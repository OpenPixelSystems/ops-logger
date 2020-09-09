project('logger','c')
inc = include_directories('include')

add_project_arguments([
  '-I' + meson.build_root(),
  ], language: 'c')

add_project_arguments(['-DCFG_LOGGER_SIMPLE_LOGGER'], language: 'c')

c_args = []
link_args = []

subdir('src')
# subdir('test')

liblogger = static_library('logger', logger_src, include_directories : inc, c_args : c_args,
  link_args : link_args,
  install : false)

logger_dep = declare_dependency(include_directories: inc, link_with: liblogger)
