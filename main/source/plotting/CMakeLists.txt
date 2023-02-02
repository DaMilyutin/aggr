cmake_minimum_required(VERSION 3.13)

add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-std=c++03>)

add_library(plotting INTERFACE ${AGGE_SOURCES})
target_include_directories(plotting INTERFACE ../../include/.)
