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
do_clang_tidy(20210308_auto
	-hicpp-move-const-arg,
	-modernize-loop-convert,
)

add_executable(20210315_literals 20210315_literals.cpp)
do_clang_tidy(20210315_literals)
if(WITH_SANITIZERS)
	target_compile_options(20210315_literals PRIVATE -fno-sanitize=undefined)
endif()

list(FIND CMAKE_CXX_COMPILE_FEATURES "cxx_std_20" index)
if(${index} GREATER -1 AND NOT CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
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
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
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
do_clang_tidy(20210517_any)
target_compile_features(20210517_any PRIVATE cxx_std_17)

if(TIPS_TESTS)
	find_program(VALGRIND_CMD NAMES valgrind)

	macro(tip_test TIP RET)
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
	endmacro()

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
endif()

