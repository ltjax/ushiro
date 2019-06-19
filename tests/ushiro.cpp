#include <catch2/catch.hpp>
#include <ushiro/state_observer.hpp>
#include <ushiro/store.hpp>

namespace {
  struct IncreaseCounter
  {
    explicit IncreaseCounter(int by=1) : by(by) {}
    int by;
  };

  struct State
  {
    int counter_ = 0;

    State apply(IncreaseCounter e) const
    {
      auto result = *this;
      result.counter_ += e.by;
      return result;     
    }
  };

}

TEST_CASE("can default construct the state")
{
  ushiro::store<State> store;
  REQUIRE(store.state.counter_ == 0);
}

TEST_CASE("can apply a simple event")
{
  ushiro::store<State> store;
  store.apply(IncreaseCounter{});
  REQUIRE(store.state.counter_ == 1);  
}

TEST_CASE("observation manager")
{
  ushiro::store<State> store;
  auto manager = std::make_shared<ushiro::observation_manager<State>>(store.state);

  // Notify about updates whenever store changes
  store.change_handler = [&](auto const& from, auto const& to) { manager->message_changed(from, to); };
  store.error_handler = [](std::exception const& e) { REQUIRE(false); };
  
  // Make sure it's not 0;
  store.apply(IncreaseCounter{42});

  auto get_counter = [](State const& s) {return s.counter_;};

  SECTION("can observe")
  {
    std::vector<int> counters;
    manager->observe(get_counter, [&](auto counter) {counters.push_back(counter);});
  
    SECTION("initial state")
    {
      REQUIRE(counters == std::vector<int>{42});
    }

    SECTION("simple changes")
    {
      store.apply(IncreaseCounter{3});
      store.apply(IncreaseCounter{5});
      REQUIRE(counters == std::vector<int>{42, 45, 50});
    }
  }

  SECTION("can add handlers in handlers")
  {
    std::vector<int> counters;

    manager->observe(get_counter, [&](int counter) {
      if (counter != 44)
        return;
      manager->observe(get_counter, [&](int counter)
      {
        counters.push_back(counter);
      });
    });
    store.apply(IncreaseCounter{2});
    store.apply(IncreaseCounter{16});
    
    REQUIRE(counters == std::vector<int>{44, 60});
  }

  SECTION("can forget immediately after observation")
  {
    auto token = manager->observe(get_counter, [&](int counter) {});
    manager->forget(token);
  }
}
