cmake_minimum_required(VERSION 2.8)
project(gcc_nop_plugin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (NOT DEFINED CMAKE_TARGET_C_COMPILER)
    set(CMAKE_TARGET_C_COMPILER ${CMAKE_C_COMPILER})
endif ()
message(STATUS "CMAKE_TARGET_C_COMPILER = ${CMAKE_TARGET_C_COMPILER}")
execute_process(
        COMMAND ${CMAKE_TARGET_C_COMPILER} -print-file-name=plugin
        OUTPUT_VARIABLE GCCPLUGINS_DIR
        RESULT_VARIABLE GCCPLUGINS_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
if (${GCCPLUGINS_RESULT} EQUAL 0)
    message(STATUS "GCCPLUGINS_DIR = ${GCCPLUGINS_DIR}")
else ()
    message(FATAL_ERROR "Could not find gcc plugin development files")
endif ()

set(TARGET gcc_nop_plugin${SUFFIX})
add_library(${TARGET} SHARED src/plugin.cpp)
target_include_directories(${TARGET} SYSTEM PRIVATE "${GCCPLUGINS_DIR}/include")
target_compile_options(${TARGET} PRIVATE -Wall -Wextra -pedantic -Werror -fno-rtti -g -O3)
if (APPLE)
    # CMake 3.13: target_link_options
    target_link_libraries(${TARGET} PRIVATE -Wl,-undefined,dynamic_lookup)
endif ()
set(PLUGIN $<TARGET_FILE:${TARGET}>)
set(PLUGIN_ID ${CMAKE_SHARED_LIBRARY_PREFIX}${TARGET})
if (APPLE)
    set(PLUGIN_ID ${PLUGIN_ID}${CMAKE_SHARED_LIBRARY_SUFFIX})
endif ()

set(HELLO_C test/hello.c)
set(HELLO_EXE test/hello${CMAKE_EXECUTABLE_SUFFIX})
add_custom_command(
        OUTPUT ${HELLO_EXE}
        COMMAND ${CMAKE_TARGET_C_COMPILER} -o ${HELLO_EXE} -fplugin=${PLUGIN} -fplugin-arg-${PLUGIN_ID}-main=4 ${HELLO_C}
        DEPENDS ${TARGET}
        IMPLICIT_DEPENDS C ${HELLO_C})
add_custom_target(
        check
        DEPENDS ${HELLO_EXE})

add_custom_target(
        fmt
        COMMAND clang-format -i src/plugin.cpp)
