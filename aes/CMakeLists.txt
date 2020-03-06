cmake_minimum_required(VERSION 3.0)

FOREACH(METHOD "MS" "AS" "QR")
  set(TESTNAME "aes-${METHOD}-${TARGET_ARCH}")
  add_executable(
      ${TESTNAME}
      main.c
      TI_aes_128_encr_only.c
      TI_aes_128_encr_only.h
      lipsum.h
  )
  include(${CMAKE_CURRENT_LIST_DIR}/../cmake/tail.cmake)
ENDFOREACH()


