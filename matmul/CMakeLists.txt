cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
    set(TESTNAME "matmul-${METHOD}")
    add_executable(
        ${TESTNAME}
        main.c
        input.h
    )
    include(${CMAKE_CURRENT_LIST_DIR}/../tail.cmake)
ENDFOREACH()

