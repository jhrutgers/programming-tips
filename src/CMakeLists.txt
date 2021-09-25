add_executable(20210125_smart_pointers 20210125_smart_pointers.cpp)
do_clang_tidy(20210125_smart_pointers
	-modernize-make-shared,
	-modernize-make-unique,
	-readability-qualified-auto,
)

add_executable(20210201_raii 20210201_raii.cpp)
do_clang_tidy(20210201_raii
	-bugprone-exception-escape,
)

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
do_clang_tidy(20210208_atomic)

add_executable(20210215_move 20210215_move.cpp)
target_compile_features(20210215_move PRIVATE cxx_std_17)
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(20210215_move PUBLIC "-Wno-self-move")
endif()
do_clang_tidy(20210215_move
	-bugprone-use-after-move,
	-clang-diagnostic-self-move,
	-hicpp-invalid-access-moved,
	-hicpp-use-emplace,
	-modernize-use-emplace,
)

add_executable(20210222_template 20210222_template.cpp)
do_clang_tidy(20210222_template
	-modernize-use-using,
)

add_executable(20210301_lambda 20210301_lambda.cpp)
do_clang_tidy(20210301_lambda)

add_executable(20210308_auto 20210308_auto.cpp)
# There seems to be some issue with clang-tidy with gcc-11 headers for C++20.
if(NOT(UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND EXISTS /usr/include/c++/11/ranges))
	do_clang_tidy(20210308_auto
		-hicpp-move-const-arg,
		-modernize-loop-convert,
	)
endif()

add_executable(20210315_literals 20210315_literals.cpp)
do_clang_tidy(20210315_literals)
if(WITH_SANITIZERS)
	target_compile_options(20210315_literals PRIVATE -fno-sanitize=undefined)
endif()

list(FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_20" index)
if(${index} GREATER -1)
	set(HAVE_CXX20 1)
endif()

if(HAVE_CXX20 AND NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_features(20210308_auto PRIVATE cxx_std_20)
	target_compile_features(20210315_literals PRIVATE cxx_std_20)
else()
	target_compile_features(20210308_auto PRIVATE cxx_std_17)
	target_compile_features(20210315_literals PRIVATE cxx_std_17)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(20210308_auto PUBLIC -Wno-unused-variable -Wno-unused-but-set-variable)
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(20210308_auto PUBLIC -Wno-unused-variable)
endif()

if(MSVC)
	target_compile_options(20210315_literals PUBLIC
		/D_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
		/D_SILENCE_CXX20_CODECVT_FACETS_DEPRECATION_WARNING
	)
endif()

add_executable(20210322_tuple 20210322_tuple.cpp)
#do_clang_tidy(20210322_tuple) # Hmm, seems to segfault on the packs...
target_compile_features(20210322_tuple PRIVATE cxx_std_17)

add_executable(20210329_variant 20210329_variant.cpp)
do_clang_tidy(20210329_variant
	-bugprone-exception-escape,
	-cppcoreguidelines-no-malloc,
	-cppcoreguidelines-pro-type-union-access,
	-cppcoreguidelines-pro-type-vararg,
	-hicpp-no-malloc,
	-hicpp-vararg
)
target_compile_features(20210329_variant PRIVATE cxx_std_17)

add_executable(20210405_pack 20210405_pack.cpp)
do_clang_tidy(20210405_pack)
target_compile_features(20210405_pack PRIVATE cxx_std_17)

add_executable(20210412_brace_init 20210412_brace_init.cpp)
do_clang_tidy(20210412_brace_init
	-clang-diagnostic-vexing-parse
)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11)
	target_compile_options(20210412_brace_init PUBLIC "-Wno-vexing-parse")
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(20210412_brace_init PUBLIC "-Wno-vexing-parse")
endif()

add_executable(20210419_enums 20210419_enums.cpp)
do_clang_tidy(20210419_enums
	-bugprone-misplaced-widening-cast,
	-clang-analyzer-deadcode.DeadStores,
	-misc-misplaced-widening-cast
)
target_compile_features(20210419_enums PRIVATE cxx_std_17)

add_executable(20210426_sfinae 20210426_sfinae.cpp)
do_clang_tidy(20210426_sfinae
	-readability-named-parameter
)
target_compile_features(20210426_sfinae PRIVATE cxx_std_17)

add_executable(20210503_bind 20210503_bind.cpp)
do_clang_tidy(20210503_bind
	-clang-analyzer-deadcode.DeadStores,
	-clang-diagnostic-mismatched-tags,
	-cppcoreguidelines-pro-bounds-constant-array-index
)
target_compile_features(20210503_bind PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(20210503_bind PUBLIC -Wno-unused-variable -Wno-unused-but-set-variable)
elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
	target_compile_options(20210503_bind PUBLIC -Wno-unused-variable)
endif()

add_executable(20210510_constexpr 20210510_constexpr.cpp)
do_clang_tidy(20210510_constexpr
	-cppcoreguidelines-pro-bounds-constant-array-index
)
if(TTM_INPUT)
	target_compile_definitions(20210510_constexpr PUBLIC "-DTTM_INPUT=${TTM_INPUT}")
endif()
target_compile_features(20210510_constexpr PRIVATE cxx_std_17)

add_executable(20210517_any 20210517_any.cpp)
target_compile_definitions(20210517_any PRIVATE -DRise_like_a_Phoenix=2014.0)
do_clang_tidy(20210517_any)
target_compile_features(20210517_any PRIVATE cxx_std_17)

add_executable(20210524_optional 20210524_optional.cpp)
do_clang_tidy(20210524_optional
	-modernize-avoid-bind,
	-bugprone-exception-escape
)
target_compile_features(20210524_optional PRIVATE cxx_std_17)

add_executable(20210531_init_list 20210531_init_list.cpp)
do_clang_tidy(20210531_init_list)
target_compile_features(20210531_init_list PRIVATE cxx_std_17)

# There is a bug in gcc 9's support of [[likely]].
# https://gcc.gnu.org/pipermail/gcc-patches/2019-September/530189.html
if(HAVE_CXX20 AND (NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 10))
	add_executable(20210607_attributes 20210607_attributes.cpp)
	do_clang_tidy(20210607_attributes
		-clang-diagnostic-unknown-attributes
	)
	target_compile_features(20210607_attributes PRIVATE cxx_std_20)

	if(MSVC)
		target_compile_options(20210607_attributes PUBLIC "/wd5030" "/wd4505")
	endif()
	if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
		target_compile_options(20210607_attributes PRIVATE "-Dsection(area)=cold")
	endif()
	if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
		target_compile_options(20210607_attributes PUBLIC "-Wno-unknown-attributes")
	endif()
endif()

add_executable(20210614_fold 20210614_fold.cpp)
do_clang_tidy(20210614_fold
	-readability-named-parameter,
)
target_compile_features(20210614_fold PRIVATE cxx_std_17)

add_executable(20210628_string_view 20210628_string_view.cpp)
do_clang_tidy(20210628_string_view)
target_compile_features(20210628_string_view PRIVATE cxx_std_17)

if(HAVE_CXX20)
	add_executable(20210927_compare 20210927_compare.cpp)
	target_compile_features(20210927_compare PRIVATE cxx_std_20)
endif()

if(TIPS_TESTS)
	find_program(VALGRIND_CMD NAMES valgrind)

	function(tip_test TIP RET)
		if(TARGET ${TIP})
			if(VALGRIND_CMD)
				add_test(NAME ${TIP}-valgrind COMMAND ${VALGRIND_CMD} --error-exitcode=1 --leak-check=full $<TARGET_FILE:${TIP}> ${ARGN})
				set_tests_properties(${TIP}-valgrind PROPERTIES PASS_REGULAR_EXPRESSION ".*ERROR SUMMARY: 0 errors.*")
			endif()

			find_program(BASH_EXE bash)

			if(UNIX AND BASH_EXE)
				if(${RET} EQUAL 0)
					add_test(NAME ${TIP} COMMAND $<TARGET_FILE:${TIP}> ${ARGN})
				else()
					add_test(NAME ${TIP} COMMAND ${BASH_EXE} -c "$<TARGET_FILE:${TIP}> ${ARGN}; echo \"exit $?\"")
					math(EXPR RET_ "${RET} % 256")
					set_tests_properties(${TIP} PROPERTIES PASS_REGULAR_EXPRESSION "exit ${RET_}")
				endif()
			else()
				add_test(NAME ${TIP} COMMAND $<TARGET_FILE:${TIP}> ${ARGN})
				if(NOT ${RET} EQUAL 0)
					set_tests_properties(${TIP} PROPERTIES WILL_FAIL 1)
				endif()
			endif()
		endif()
	endfunction()

	tip_test(20210125_smart_pointers 47)
	tip_test(20210201_raii 0)
	tip_test(20210208_atomic 0)
	tip_test(20210215_move 0)
	tip_test(20210222_template 0)
	tip_test(20210301_lambda 955 1000)
	tip_test(20210308_auto 0)
	tip_test(20210315_literals 0)
	tip_test(20210322_tuple 0)
	tip_test(20210329_variant 0)
	tip_test(20210405_pack 0)
	tip_test(20210412_brace_init 0)
	tip_test(20210419_enums 2)
	tip_test(20210426_sfinae 0)
	tip_test(20210503_bind 0)
	tip_test(20210510_constexpr 0 'IBM_d1d_it!1')
	tip_test(20210517_any 12)
	tip_test(20210524_optional 0)
	tip_test(20210531_init_list 0)
	tip_test(20210607_attributes 0)
	tip_test(20210614_fold 0)
	tip_test(20210628_string_view 0)
	tip_test(20210927_compare 0)
endif()

