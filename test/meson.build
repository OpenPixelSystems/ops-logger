test_c_args = [c_args, '-DCFG_LOGGER_SIMPLE_LOGGER']
logger_v3 = executable('logger_v3_test','logger_v3_test.c', logger_srcs,
			include_directories:logger_includes,
			# dependencies :  deps,
			c_args : test_c_args,
			link_args : link_args)
test('Main test', logger_v3)
