#pragma once

#include "event_bus.hpp"
#include <memory>

namespace ushiro
{
namespace detail
{
template <typename T> struct recursive_event_binder
{
};

template <> struct recursive_event_binder<std::tuple<>>
{
    template <class Store> static ushiro::event_bus::subscription_list bind(ushiro::event_bus& bus, Store& store)
    {
        return {};
    }
};

template <typename Head, typename... Tail> struct recursive_event_binder<std::tuple<Head, Tail...>>
{
    template <class Store> static ushiro::event_bus::subscription_list bind(ushiro::event_bus& bus, Store& store)
    {
        auto subscriptions = recursive_event_binder<std::tuple<Tail...>>::bind(bus, store);
        subscriptions += bus.subscribe<Head>(event_applier(store));
        return subscriptions;
    }
};

} // namespace detail

template <class Store> ushiro::event_bus::subscription_list subscribe_event_list(ushiro::event_bus& bus, Store& store)
{
    return detail::recursive_event_binder<typename Store::state_type::event_list>::bind(bus, store);
}

} // namespace ushiro