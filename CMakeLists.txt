cmake_minimum_required(VERSION 3.12)
project(gcc_nop_plugin)

set(CMAKE_CXX_STANDARD 98)

execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=plugin
        OUTPUT_VARIABLE GCCPLUGINS_DIR
        RESULT_VARIABLE GCCPLUGINS_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
if(${GCCPLUGINS_RESULT} EQUAL 0)
    message(STATUS "GCCPLUGINS_DIR = ${GCCPLUGINS_DIR}")
else()
    message(FATAL_ERROR "Could not find gcc plugin development files")
endif()

add_library(gcc_nop_plugin SHARED src/plugin.cpp)
target_include_directories(gcc_nop_plugin SYSTEM PRIVATE "${GCCPLUGINS_DIR}/include")
target_compile_options(gcc_nop_plugin PRIVATE -Wall -Wextra -pedantic -Werror -fno-rtti -g -O3)
# CMake 3.13: target_link_options
if(APPLE)
    target_link_libraries(gcc_nop_plugin PRIVATE -Wl,-undefined,dynamic_lookup)
endif()
set(PLUGIN $<TARGET_FILE_NAME:gcc_nop_plugin>)

set(HELLO_CPP test/hello.cpp)
set(HELLO_EXE test/hello${CMAKE_EXECUTABLE_SUFFIX})
add_custom_command(
        OUTPUT ${HELLO_EXE}
        COMMAND ${CMAKE_CXX_COMPILER} -o ${HELLO_EXE} -fplugin=${PLUGIN} -fplugin-arg-${PLUGIN}-_main=2 ${HELLO_CPP}
        DEPENDS gcc_nop_plugin
        IMPLICIT_DEPENDS CXX ${HELLO_CPP})
add_custom_target(
        check
        DEPENDS ${HELLO_EXE})
