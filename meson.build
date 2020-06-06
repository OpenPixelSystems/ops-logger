project('logger','c')
inc = include_directories('include')

add_project_arguments([
  '-I' + meson.build_root(),
], language: 'c')

add_project_arguments(['-DCFG_LOGGER_USE_BUILTIN'], language: 'c')
add_project_arguments(['-DCFG_LOGGER_MAX_LOG_SIZE=10'], language: 'c')
add_project_arguments(['-DCFG_LOGGER_SPLIT_ERROR_LOGS'], language: 'c')
#add_project_arguments(['-DCFG_LOGGER_DEEP_EMBEDDED'], language: 'c')
add_project_arguments(['-D_GNU_SOURCE'], language: 'c')


subdir('src')
subdir('test')
