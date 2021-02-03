add_executable(20210125_smart_pointers 20210125_smart_pointers.cpp)
add_executable(20210201_raii 20210201_raii.cpp)

add_executable(20210208_atomic 20210208_atomic.cpp)
find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
	target_compile_options(20210208_atomic PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
	target_link_libraries(20210208_atomic "${CMAKE_THREAD_LIBS_INIT}")
endif()
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_link_libraries(20210208_atomic atomic)
endif()

add_executable(20210215_move 20210215_move.cpp)
set_property(TARGET 20210215_move PROPERTY CXX_STANDARD 17)
if(MSVC)
	target_compile_options(20210215_move PUBLIC "/Zc:__cplusplus")
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(20210215_move PUBLIC "-Wno-self-move")
endif()

add_executable(20210222_template 20210222_template.cpp)

add_executable(20210222_template 20210222_template.cpp)


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
	tip_test(20210208_atomic)
	tip_test(20210215_move)
	tip_test(20210222_template)
endif()

