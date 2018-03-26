cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "/usr/bin/clang-5.0")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(ced)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

#Dirs
set(LIBDIR "libs")
set(BUILDDIR "build")
set(SRCDIR "src")

include_directories(SRCDIR)

add_compile_options(-g -Wall -std=c++17)

add_executable(debug
	${SRCDIR}/term.cc
        ${SRCDIR}/ced.cc
        ${SRCDIR}/main.cc
        ${SRCDIR}/log.cc
        ${SRCDIR}/common.cc
        ${SRCDIR}/fileutils.cc
        ${SRCDIR}/commands.cc
        ${SRCDIR}/buffer.cc
        ${SRCDIR}/buffer_view.cc
        ${SRCDIR}/window.cc
        ${SRCDIR}/input.cc
	${SRCDIR}/draw.cc
)

target_link_libraries(debug ncursesw)
