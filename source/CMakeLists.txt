set(ushiro_HEADERS
  ushiro/event_bus.hpp
  ushiro/subscribe_event_list.hpp
  ushiro/state_observer.hpp
  ushiro/store.hpp
  ushiro/diffable_list.hpp)

add_library(ushiro
  ushiro/event_bus.cpp
  ushiro/event_bus.hpp
  ushiro/subscribe_event_list.hpp
  ushiro/state_observer.hpp
  ushiro/store.hpp
  ushiro/diffable_list.hpp)

target_include_directories(ushiro
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
            $<INSTALL_INTERFACE:include/>)

set_target_properties(ushiro PROPERTIES
  PUBLIC_HEADER "${ushiro_HEADERS}")

install(TARGETS ushiro
  EXPORT ushiro-export
  LIBRARY DESTINATION lib/
  ARCHIVE DESTINATION lib/
  PUBLIC_HEADER DESTINATION include/ushiro/)

install(FILES)

install(EXPORT ushiro-export
  FILE ushiro-targets.cmake
  NAMESPACE ushiro::
  DESTINATION cmake/)
