cmake_minimum_required(VERSION 3.5)
project(chess)

set(SOURCE 
  ./src/main.c
  ./src/check.c
  ./src/rendering.c
  ./src/utility.c
  ./src/validators.c
  ./src/globals.c
)

add_executable(main ${SOURCE})
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})
target_link_libraries(main ${CURSES_LIBRARY})
