cmake_minimum_required(VERSION 3.13)

project(agge)

enable_testing()

option(AGGE_NO_TESTS "Do not build test modules." OFF)
option(AGGE_NO_EXTENSIONS "Do not build/test extensions (like async)." OFF)
option(AGGE_NO_SAMPLES "Do not build sample applications." OFF)

option(AGGE_NO_SAMPLES_COMMON "Do not build samples.common library." OFF)

set (CMAKE_CXX_STANDARD          20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)

add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)

set(GCC_CLANG_COMPILER_OPTIONS_CXX -std=c++20 -fPIC -Wall -fno-rtti)
set(GCC_CLANG_COMPILER_OPTIONS_CXX ${GCC_CLANG_COMPILER_OPTIONS_CXX}
	-Wall -Wextra -Wpedantic
	-Wno-missing-field-initializers
	-Wno-implicit-fallthrough
)
if(NOT ANDROID_ABI OR ANDROID_ABI MATCHES "x86.*")
	set(GCC_CLANG_COMPILER_OPTIONS_CXX ${GCC_CLANG_COMPILER_OPTIONS_CXX} -msse2)
endif()

add_compile_options(
	"$<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:${GCC_CLANG_COMPILER_OPTIONS_CXX}>"
	"$<$<CXX_COMPILER_ID:MSVC>:-W4;-wd4512;-wd4355>"
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release,RelWithDebInfo>:-Ox;-Ob2>>"
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<EQUAL:4,${CMAKE_SIZEOF_VOID_P}>:-arch:SSE2>>"
)

if (GNU)
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
endif()

if (NOT TARGET utfia)
	set(UTFIA_NO_TESTS ON)
	add_subdirectory(libraries/utfia)
endif()

add_subdirectory(main/source/agge)
add_subdirectory(main/source/agge.text)
if (NOT AGGE_NO_EXTENSIONS AND (NOT MSVC_VERSION OR (MSVC_VERSION GREATER_EQUAL 1600)))
	if (NOT TARGET polyq)
		set(POLYQ_NO_TESTS ON)
		add_subdirectory(libraries/polyq)
	endif()
	add_library(agge.async INTERFACE)
	target_include_directories(agge.async INTERFACE ./main/include)
	target_link_libraries(agge.async INTERFACE agge INTERFACE polyq)
endif()

if (NOT AGGE_NO_TESTS)
	if (NOT TARGET utee)
		set(UTEE_NO_TESTS ON)
		add_subdirectory(libraries/utee)
	endif()
	add_subdirectory(test/tests)
	add_utee_test(agge.tests)
	add_utee_test(agge.text.tests)
	if (TARGET agge.async.tests)
		add_utee_test(agge.async.tests)
	endif()
endif()

if (NOT AGGE_NO_SAMPLES_COMMON)
    add_subdirectory(libraries/system_events)
    add_subdirectory(test/samples/common/src)
endif()

if (NOT AGGE_NO_SAMPLES)
	if (NOT TARGET strmd)
		set(STRMD_NO_TESTS ON)
		add_subdirectory(libraries/strmd)
	endif()
	if (NOT TARGET ylems)
		set(YLEMS_NO_TESTS ON)
		add_subdirectory(libraries/ylems)
	endif()
    add_subdirectory(libraries/system_events)
	add_subdirectory(test/samples)
endif()


