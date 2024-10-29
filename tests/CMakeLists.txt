
add_executable(tests
  tests_main.cpp
  ushiro.t.cpp)

target_link_libraries(tests
  PUBLIC Catch2::Catch2
  PUBLIC ushiro)