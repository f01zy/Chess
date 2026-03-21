cmake_minimum_required(VERSION 3.5)
project(chess)

set(SOURCE 
  ./src/main.c
  ./src/globals.c
  ./src/engine/check.c
  ./src/engine/utility.c
  ./src/engine/validators.c
  ./src/game/rendering.c
  ./src/game/game.c
  ./src/game/ui.c
  ./src/network/socket.c
  ./third_party/mongoose/mongoose.c
)

add_executable(main ${SOURCE})
find_package(Curses REQUIRED)

include_directories(${CURSES_INCLUDE_DIR})
include_directories(./third_party/mongoose)
target_link_libraries(main PRIVATE ${CURSES_LIBRARY})
