if meson.is_cross_build()
main_test = executable('logger_test','main.c',
			include_directories:inc,
			dependencies :  deps,
			c_args : c_args,
			link_args : link_args,
			link_with: loggerlib)
else

endif
test('Main test', main_test)
