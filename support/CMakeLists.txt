cmake_minimum_required(VERSION 3.13)

add_library(support-${TARGET_ARCH} "")

target_sources(support-${TARGET_ARCH}
  PRIVATE cm0-vectors.c cm0-support.c
  PUBLIC cm0.h
  )
