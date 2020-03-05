cmake_minimum_required(VERSION 3.13)

# Commmon function to add linker script and definitions for each target
  message("Testname ${TESTNAME}")

target_link_libraries(
  ${TESTNAME}
  LINK_PUBLIC ic-${TARGET_ARCH}-${METHOD}
  ${SUPPORT_LIBS}
  )

set_target_properties(${TESTNAME} PROPERTIES SUFFIX ".elf")

IF(${TARGET_ARCH} STREQUAL "msp430")
  #add_msp_upload(${TESTNAME})
  IF (${METHOD} STREQUAL "QR")
      target_link_options(
          ${TESTNAME}
          PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/support/msp430fr5994-fram-only.ld
          )
  ELSE ()
      target_link_options(
          ${TESTNAME}
          PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/support/msp430fr5994.ld
          )
  ENDIF()

ELSEIF(${TARGET_ARCH} STREQUAL "cm0")
  message("Testname ${TESTNAME}")
  target_link_options(
      ${TESTNAME}
      PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/support/cm0-FS.ld
      )
ENDIF()

# Emit map, listing and hex
add_custom_command(TARGET ${TESTNAME} POST_BUILD
  COMMAND ${TC-SIZE} -A -x "$<TARGET_FILE:${TESTNAME}>" > ${TESTNAME}.map
  COMMAND ${TC-OBJDUMP} -d "$<TARGET_FILE:${TESTNAME}>" > ${TESTNAME}.lst
  COMMAND ${TC-OBJCOPY} -O ihex "$<TARGET_FILE:${TESTNAME}>" ${TESTNAME}.hex
  )

# add upload target
add_upload(${TESTNAME})
