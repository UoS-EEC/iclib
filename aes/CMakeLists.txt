cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
    set(TESTNAME "aes-${METHOD}")
    add_executable(
        ${TESTNAME}
        main.c
        TI_aes_128_encr_only.c
        TI_aes_128_encr_only.h
        lipsum.h
    )
    include(${CMAKE_CURRENT_LIST_DIR}/../tail.cmake)
ENDFOREACH()


