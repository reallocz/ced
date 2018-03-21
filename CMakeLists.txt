cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "/usr/bin/clang-5.0")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

project(ced)


#Dirs
set(LIBDIR "libs")
set(BUILDDIR "build")
set(SRCDIR "src")

include_directories(SRCDIR)

add_compile_options(-g -Wall)

add_executable(debug
        ${SRCDIR}/log.c
        ${SRCDIR}/common.c
        ${SRCDIR}/fileutils.c
        ${SRCDIR}/buffer.c
        ${SRCDIR}/buffer_view.c
        ${SRCDIR}/window.c
        ${SRCDIR}/input.c
	${SRCDIR}/term.c
	${SRCDIR}/draw.c
        ${SRCDIR}/ced.c
        ${SRCDIR}/main.c
)

target_link_libraries(debug ncurses)
