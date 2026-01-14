add_library(chess_engine STATIC
  src/engine.cpp
)

target_include_directories(chess_engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (MSVC)
  target_compile_options(chess_engine PRIVATE /W4 /permissive-)
else()
  target_compile_options(chess_engine PRIVATE -Wall -Wextra -Wpedantic)
endif()
