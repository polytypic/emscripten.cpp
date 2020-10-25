if(NOT EMSCRIPTEN)
  message(WARNING "`emscripten_v1` requires Emscripten.")
  return()
endif()

add_conventional_executable_tests(PRIVATE emscripten_v1)
