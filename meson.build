project('logger','c')
inc = include_directories('include')

add_project_arguments([
  '-I' + meson.build_root(),
], language: 'c')

# add_project_arguments(['-DCFG_LOGGER_DEEP_EMBEDDED'], language: 'c')
# add_project_arguments(['-DCFG_LOGGER_NO_MALLOC'], language: 'c')
# add_project_arguments(['-DCFG_LOGGER_USE_BUILTIN'], language: 'c')
# add_project_arguments(['-DCFG_LOGGER_MAX_LOG_SIZE=10'], language: 'c')
# add_project_arguments(['-DCFG_LOGGER_SPLIT_ERROR_LOGS'], language: 'c')
# add_project_arguments(['-D_GNU_SOURCE'], language: 'c')

add_project_arguments(['-DCFG_LOGGER_SIMPLE_LOGGER'], language: 'c')

c_args = []
link_args = []

if meson.is_cross_build()
    if host_machine.system() == 'none'
	add_project_arguments(['-DCFG_LOGGER_DEEP_EMBEDDED'], language: 'c')
	add_project_arguments(['-DCFG_LOGGER_NO_MALLOC'], language: 'c')

    endif
    c_args += meson.get_cross_property('cross_args')
    link_args += meson.get_cross_property('cross_link_args')
endif

subdir('src')
subdir('test')
