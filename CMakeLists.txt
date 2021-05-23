cmake_minimum_required(VERSION 3.0)
project(programming-hints)

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.10.0")
	option(TIPS_TESTS "Build the tests" ON)
	if(TIPS_TESTS)
		if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.17.0")
			set(CTEST_OUTPUT_ON_FAILURE 1)
			list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
		endif()

		enable_testing()
	endif()
endif()

if(MSVC)
	add_compile_options(/W4 /WX /wd4100 /wd4930 /Zc:__cplusplus)
else()
	add_compile_options(-Wall -Wextra -Werror -Wdouble-promotion -Wformat=2 -Wundef -Wconversion -Wno-unused-parameter -Wshadow)
endif()

if(${CMAKE_VERSION} VERSION_GREATER "3.6.0")
	find_program(CLANG_TIDY_EXE NAMES "clang-tidy" DOC "Path to clang-tidy executable")
endif()

set(WITH_SANITIZERS_DEFAULT OFF)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(WITH_SANITIZERS_DEFAULT ON)
	elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
		set(WITH_SANITIZERS_DEFAULT ON)
	endif()
endif()

option(WITH_SANITIZERS "Build with address, leak and undefined sanitizers" ${WITH_SANITIZERS_DEFAULT})

if(WITH_SANITIZERS)
	if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" AND APPLE)
		add_compile_options( -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer)
		set(SANITIZER_FLAGS "-fsanitize=address -fsanitize=undefined")
	else()
		add_compile_options( -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer)
		set(SANITIZER_FLAGS "-fsanitize=address -fsanitize=leak -fsanitize=undefined")
	endif()
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${SANITIZER_FLAGS}")
endif()

function(do_clang_tidy target)
	if(CLANG_TIDY_EXE)
		string(CONCAT CLANG_TIDY_CHECKS "-checks="
			"bugprone-*,"
			"clang-analyzer-*,"
			"concurrency-*,"
			"cppcoreguidelines-*,"
			"-cppcoreguidelines-avoid-c-arrays,"
			"-cppcoreguidelines-avoid-magic-numbers,"
			"-cppcoreguidelines-non-private-member-variables-in-classes,"
			"-cppcoreguidelines-owning-memory,"
			"-cppcoreguidelines-pro-bounds-array-to-pointer-decay,"
			"-cppcoreguidelines-pro-bounds-pointer-arithmetic,"
			"hicpp-*,"
			"-hicpp-avoid-c-arrays,"
			"-hicpp-braces-around-statements,"
			"-hicpp-exception-baseclass,"
			"-hicpp-explicit-conversions,"
			"-hicpp-no-array-decay,"
			"misc-*,"
			"-misc-non-private-member-variables-in-classes,"
			"-misc-unused-parameters,"
			"modernize-*,"
			"-modernize-avoid-c-arrays,"
			"-modernize-pass-by-value,"
			"-modernize-use-nodiscard,"
			"-modernize-use-trailing-return-type,"
			"-modernize-unary-static-assert,"
			"portability-*,"
			"readability-*,"
			"-readability-braces-around-statements,"
			"-readability-convert-member-functions-to-static,"
			"-readability-implicit-bool-conversion,"
			"-readability-magic-numbers,"
			${ARGN}
		)
		set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "${CLANG_TIDY_CHECKS}"
			-warnings-as-errors=*
			-extra-arg=-Wno-unknown-warning-option
		)
		set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${DO_CLANG_TIDY}")
	endif()
endfunction()

add_subdirectory(src)

find_program(CPPCHECK_CMD NAMES cppcheck)
if(CPPCHECK_CMD)
	add_custom_target(cppcheck ALL
		COMMAND
			${CPPCHECK_CMD}
			--enable=warning,style,information
			--force
			--inline-suppr
			--quiet
			--suppressions-list=${CMAKE_CURRENT_SOURCE_DIR}/.cppcheck_suppr
#			--xml
			src
			-isrc/20210510_constexpr.cpp
			-isrc/20210524_optional.cpp
#			--check-config
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM
	)
endif()

