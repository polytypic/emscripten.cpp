if(NOT EMSCRIPTEN)
  message(WARNING "`emscripten_v1` requires Emscripten.")
  return()
endif()

add_conventional_library(emscripten_v1)
target_link_libraries(emscripten_v1
  INTERFACE emscripten_system_include
  PUBLIC data_v1)
