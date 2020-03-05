cmake_minimum_required(VERSION 3.13)

add_library(support "")

target_sources(support
  PRIVATE cm0-vectors.c
  PUBLIC cm0.h
  )
