add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib raylib_build)

add_executable(chess_gui
  src/main.cpp
)

target_link_libraries(chess_gui PRIVATE chess_engine raylib)

# Helps MinGW find runtime deps at run time when launching from build dir
set_target_properties(chess_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gui)
