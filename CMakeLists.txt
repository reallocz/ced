cmake_minimum_required(VERSION 3.0)
project(ced)

#Dirs
set(LIBDIR "libs")
set(BUILDDIR "build")
set(SRCDIR "src")

include_directories(SRCDIR)

add_compile_options(-g -Wall -Wextra -Wpedantic)

add_executable(debug
        ${SRCDIR}/main.c
        ${SRCDIR}/fileutils.c
        ${SRCDIR}/log.c
        ${SRCDIR}/window.c
        ${SRCDIR}/input.c
        ${SRCDIR}/buffer.c
        ${SRCDIR}/menu.c
	${SRCDIR}/term.c
        ${SRCDIR}/ced.c
)

target_link_libraries(debug ncurses)
