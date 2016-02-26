if(NOT DISABLE_UNITTEST AND Boost_FOUND)
	enable_testing()

	add_executable(
	  test_slog
	  test_tag.cc
	  test_slog.cc
	)
	target_link_libraries(
	  test_slog
    slog
  	${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
	)
	add_test(test_slog test_slog)
endif (NOT DISABLE_UNITTEST AND Boost_FOUND)
