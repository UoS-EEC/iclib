cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
    set(TESTNAME "crc-${METHOD}")
    add_executable(
        ${TESTNAME}
        main.c
        crc.h
        crc_32.c
        lipsum.h
        sniptype.h
    )
    include(${CMAKE_CURRENT_LIST_DIR}/../tail.cmake)
ENDFOREACH()

