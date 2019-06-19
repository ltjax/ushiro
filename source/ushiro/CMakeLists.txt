add_library(ushiro
  event_bus.cpp
  event_bus.hpp
  subscribe_event_list.hpp
  state_observer.hpp
  store.hpp
  diffable_list.hpp)

target_include_directories(ushiro
  INTERFACE ../)
