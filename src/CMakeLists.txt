add_executable(20210125_smart_pointers 20210125_smart_pointers.cpp)
add_executable(20210201_raii 20210201_raii.cpp)

if(TIPS_TESTS)
	find_program(VALGRIND_CMD NAMES valgrind)

	macro(tip_test TIP)
		if(VALGRIND_CMD)
			add_test(NAME ${TIP}-valgrind COMMAND ${VALGRIND_CMD} --error-exitcode=1 --leak-check=full $<TARGET_FILE:${TIP}>)
			set_tests_properties(${TIP}-valgrind PROPERTIES PASS_REGULAR_EXPRESSION ".*ERROR SUMMARY: 0 errors.*")
		endif()
	endmacro()

	tip_test(20210125_smart_pointers)
	tip_test(20210201_raii)
endif()

