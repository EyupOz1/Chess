file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_library(chess_engine STATIC ${ENGINE_SRC})


target_include_directories(chess_engine
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(chess_engine_tests tests/engine_tests.cpp)
target_link_libraries(chess_engine_tests PRIVATE chess_engine)

if (MSVC)
  target_compile_options(chess_engine PRIVATE /W4 /permissive- /std:c++latest)
else()
  target_compile_options(chess_engine PRIVATE -Wall -Wextra -Wpedantic -std=c++23)
endif()

