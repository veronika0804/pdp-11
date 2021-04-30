cmake_minimum_required(VERSION 3.17)
project(pdp11 C)

set(CMAKE_C_STANDARD 99)

add_executable(pdp11 pdp_mem.c pdp_run.c pdp11.h)
