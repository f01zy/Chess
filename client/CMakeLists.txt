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
add_subdirectory(./third_party/cjson)
find_package(Curses REQUIRED)

target_include_directories(main PRIVATE
  ${CURSES_INCLUDE_DIR}
  ./third_party/cjson
  ./third_party/mongoose
)

target_link_libraries(main PRIVATE 
  ${CURSES_LIBRARY}
  cjson
)
