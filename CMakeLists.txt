cmake_minimum_required(VERSION 3.0)
project(ced)

#Dirs
set(LIBDIR "libs")
set(BUILDDIR "build")
set(SRCDIR "src")

include_directories(SRCDIR)

add_compile_options(-g -Wall -Wextra -Wpedantic)

add_executable(debug
        ${SRCDIR}/log.c
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
