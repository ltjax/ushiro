#pragma once
#include <functional>

namespace ushiro
{
template<typename T>
struct store
{
  using state_type = T;
  T state;

  std::function<void(T const&, T const&)> change_handler;
  std::function<void(std::exception const&)> error_handler;

  template<typename event_type>
  void apply(event_type const& event)
  {
    // Order is important here to guarantee object lifetimes
    auto last = std::move(state);
    try
    {
      state = last.apply(event);
      if (change_handler)
      {
        change_handler(last, state);
      }
    }
    catch(std::exception const& e)
    {
      if (!error_handler)
        throw;
      error_handler(e);
    }
  }
};

template <class ModelType>
struct event_application_function
{
  explicit event_application_function(store<ModelType>& store) : m_store(&store) {}

  template <typename Event>
  void operator()(Event const& event) const
  {
    m_store->apply(event);
  }
private:
  store<ModelType>* m_store;
};


template <class ModelType>
event_application_function<ModelType> event_applier(store<ModelType>& store)
{
  return event_application_function<ModelType>(store);
}

}
