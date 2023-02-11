cmake_minimum_required(VERSION 3.13)

add_compile_options($<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-std=c++03>)


set(PLOTTING_COMMON_SOURCES ${PLOTTING_COMMON_SOURCES}
    common/font_loader.cpp
    common/truetype.cpp
)

if (WIN32)
    set(PLOTTING_COMMON_SOURCES ${PLOTTING_COMMON_SOURCES}
    common/platform/win32/services.cpp)
endif()

add_library(plotting STATIC ${PLOTTING_COMMON_SOURCES})

target_include_directories(plotting INTERFACE ../../include/.)
target_include_directories(plotting INTERFACE ../../../libraries/strmd)
target_link_libraries(plotting agge agge.text ylems strmd)
