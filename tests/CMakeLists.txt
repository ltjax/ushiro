
add_executable(tests
  tests_main.cpp
  ushiro.cpp)

target_link_libraries(tests
  PUBLIC CONAN_PKG::Catch2
  PUBLIC ushiro)