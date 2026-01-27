add_subdirectory(${CMAKE_SOURCE_DIR}/external/raylib raylib_build)

file(GLOB_RECURSE GUI_SRC CONFIGURE_DEPENDS
     ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

add_executable(chess_gui ${GUI_SRC})

target_link_libraries(chess_gui PRIVATE chess_engine raylib)
target_include_directories(chess_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Helps MinGW find runtime deps at run time when launching from build dir
set_target_properties(chess_gui PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/gui)

if (${PLATFORM} MATCHES "Web")
  set_target_properties(chess_gui PROPERTIES SUFFIX ".html")
endif()
