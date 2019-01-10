cmake_minimum_required(VERSION 3.11)
project(gcc_nop_plugin)

set(CMAKE_CXX_STANDARD 11)

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

add_library(gcc_nop_plugin SHARED src/plugin.cpp)
target_include_directories(gcc_nop_plugin SYSTEM PRIVATE "${GCCPLUGINS_DIR}/include")
target_compile_options(gcc_nop_plugin PRIVATE -Wall -Wextra -pedantic -Werror -fno-rtti -g -O3)
if (APPLE)
    # CMake 3.13: target_link_options
    target_link_libraries(gcc_nop_plugin PRIVATE -Wl,-undefined,dynamic_lookup)
endif ()
set(PLUGIN $<TARGET_FILE:gcc_nop_plugin>)
set(PLUGIN_ID ${CMAKE_SHARED_LIBRARY_PREFIX}gcc_nop_plugin)
if (APPLE)
    set(PLUGIN_ID ${PLUGIN_ID}${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

set(HELLO_C test/hello.c)
set(HELLO_EXE test/hello${CMAKE_EXECUTABLE_SUFFIX})
add_custom_command(
        OUTPUT ${HELLO_EXE}
        COMMAND ${CMAKE_TARGET_C_COMPILER} -o ${HELLO_EXE} -fplugin=${PLUGIN} -fplugin-arg-${PLUGIN_ID}-main=2 ${HELLO_C}
        DEPENDS gcc_nop_plugin
        IMPLICIT_DEPENDS C ${HELLO_C})
add_custom_target(
        check
        DEPENDS ${HELLO_EXE})
