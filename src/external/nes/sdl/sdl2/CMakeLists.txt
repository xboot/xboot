cmake_minimum_required(VERSION 3.10)

project(nes)

find_package(SDL2 CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(NES_DIRS "../..")

file(GLOB_RECURSE SRCS
        ${NES_DIRS}/src/*.c
        port/*.c
        main.c
)
list(APPEND INCS
        ${NES_DIRS}/inc
        port
)
include_directories(${INCS})

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME}
        PRIVATE SDL2::SDL2
)
