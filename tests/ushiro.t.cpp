#include <catch2/catch.hpp>
#include <ushiro/state_observer.hpp>
#include <ushiro/store.hpp>

namespace
{

struct increase_counter
{
  explicit increase_counter(int by = 1)
  : by(by)
  {
  }
  int by;
};

struct test_event
{
};

struct insert_event
{
    int value;
};

struct counting_state
{
  int counter_ = 0;

  counting_state apply(increase_counter e) const
  {
    auto result = *this;
    result.counter_ += e.by;
    return result;
  }
};

struct container_state
{
  std::vector<int> ints_;

  container_state apply(test_event) const
  {
    auto copy = *this;
    copy.ints_.push_back(1);
    return copy;
  }
};

struct throwing_state
{
    std::vector<int> ints_;

    throwing_state apply(insert_event e) const
    {
        auto result = *this;
        result.ints_.push_back(e.value);
        return result;
    }

    throwing_state apply(test_event) const
    {
        throw std::runtime_error("the_error_message");
    }
};

}

TEST_CASE("can default construct the state")
{
  ushiro::store<counting_state> store;
  REQUIRE(store.state.counter_ == 0);
}

TEST_CASE("can apply a simple event")
{
  ushiro::store<counting_state> store;
  store.apply(increase_counter{});
  REQUIRE(store.state.counter_ == 1);
}

TEST_CASE("exception handling during application")
{
    ushiro::store<throwing_state> store;

    SECTION("falls thru without error_handler")
    {
        REQUIRE_THROWS_AS(store.apply(test_event{}), std::runtime_error);
    }
    SECTION("calls error_handler when set")
    {
        using namespace std::string_literals;
        bool was_called = false;
        store.error_handler = [&](std::exception const& e)
        {
            was_called = true;
            REQUIRE(e.what() == "the_error_message"s);
        };
        REQUIRE(!was_called);
        store.apply(test_event{});
        REQUIRE(was_called);
    }
    SECTION("state stays unchanged after error")
    {
        store.apply(insert_event{ 33 });
        REQUIRE(store.state.ints_ == std::vector<int>{ 33});
        REQUIRE_THROWS_AS(store.apply(test_event{}), std::runtime_error);
        REQUIRE(store.state.ints_ == std::vector<int>{ 33});
    }
}

TEST_CASE("changed_handler")
{
  ushiro::store<container_state> Store;

  SECTION("gets called")
  {
    auto wasCalled = false;
    Store.change_handler = [&](auto const&, auto const&) { wasCalled = true; };

    Store.apply(test_event{});
    REQUIRE(wasCalled);
  }

  SECTION("next object is identical")
  {
    std::vector<int> const* buffer = nullptr;
    Store.change_handler = [&](auto const&, auto const& next) {
      // Make sure that it is valid to store pointers into the current store
      buffer = &next.ints_;
    };
    Store.apply(test_event{});
    REQUIRE(buffer == &Store.state.ints_);
  }
}

TEST_CASE("observation manager")
{
  ushiro::store<counting_state> store;
  auto manager = std::make_shared<ushiro::observation_manager<counting_state>>(store.state);

  // Notify about updates whenever store changes
  store.change_handler = [&](auto const& from, auto const& to) { manager->message_changed(from, to); };
  store.error_handler = [](std::exception const& e) { REQUIRE(false); };

  // Make sure it's not 0;
  store.apply(increase_counter{ 42 });

  auto get_counter = [](counting_state const& s) { return s.counter_; };

  SECTION("can observe")
  {
    std::vector<int> counters;
    manager->observe(get_counter, [&](auto counter) { counters.push_back(counter); });

    SECTION("initial state")
    {
      REQUIRE(counters == std::vector<int>{ 42 });
    }

    SECTION("simple changes")
    {
      store.apply(increase_counter{ 3 });
      store.apply(increase_counter{ 5 });
      REQUIRE(counters == std::vector<int>{ 42, 45, 50 });
    }
  }

  SECTION("can add handlers in handlers")
  {
    std::vector<int> counters;

    manager->observe(get_counter, [&](int counter) {
      if (counter != 44)
        return;
      manager->observe(get_counter, [&](int counter) { counters.push_back(counter); });
    });
    store.apply(increase_counter{ 2 });
    store.apply(increase_counter{ 16 });

    REQUIRE(counters == std::vector<int>{ 44, 60 });
  }

  SECTION("can forget immediately after observation")
  {
    auto token = manager->observe(get_counter, [&](int counter) {});
    manager->forget(token);
  }
}
