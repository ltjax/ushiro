#pragma once

#include <functional>
#include <list>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ushiro
{
class event_bus
{
public:
    class event_wrapper
    {
    public:
        virtual ~event_wrapper() = default;
    };

    event_bus();
    ~event_bus();

    template <typename T> struct parameter_for_signature;
    template <typename T, typename R, typename P> struct parameter_for_signature<R (T::*)(P) const>
    {
        using type = P;
    };
    template <typename T> struct parameter_for_callable
    {
        using type = typename parameter_for_signature<decltype(&T::operator())>::type;
    };

    template <typename SelectedEvent, typename Handler> struct event_type_for
    {
        using type = SelectedEvent;
    };

    template <typename Handler> struct event_type_for<void, Handler>
    {
        using type = typename std::decay<typename parameter_for_callable<Handler>::type>::type;
    };

    using WrappedEventHandler = std::function<void(event_wrapper const& wrapper)>;
    using handler_list_t = std::list<WrappedEventHandler>;
    using handler_map_t = std::unordered_map<std::type_index, handler_list_t>;

    struct handle
    {
        std::type_index type;
        handler_list_t::iterator position;
    };

    template <class Event, class... Args> void dispatch(Args&&... args)
    {
        auto found = m_handlers.find(typeid(Event));
        if (found == m_handlers.end() || found->second.empty())
        {
            return;
        }

        auto wrappedEvent = concrete_wrapper<Event>(std::forward<Args>(args)...);
        for (auto const& handler : found->second)
        {
            handler(wrappedEvent);
        }
    }

    template <class Event = void, class Handler> handle attach(Handler handler)
    {
        using event_type = typename event_type_for<Event, Handler>::type;

        auto key = std::type_index(typeid(event_type));

        auto wrappedHandler = [handler](event_wrapper const& wrapper) {
            auto const& event = static_cast<concrete_wrapper<event_type> const&>(wrapper).get();
            handler(event);
        };
        auto& list = m_handlers[key];
        auto position = list.insert(list.end(), wrappedHandler);
        return { key, position };
    }

    void detach(handle subscription)
    {
        m_handlers[subscription.type].erase(subscription.position);
    }

    /** Behaves like a resource. It's movable, but not copyable.
     */
    class subscription
    {
    public:
        subscription()
        : parent(nullptr)
        , actual({ typeid(void), {} })
        {
        }

        subscription(subscription const&) = delete;

        subscription(event_bus* parent, handle actual)
        : parent(parent)
        , actual(actual)
        {
        }

        subscription(subscription&& rhs) noexcept
        : parent(rhs.parent)
        , actual(rhs.actual)
        {
            rhs.parent = nullptr;
        }

        void operator=(subscription const&) = delete;
        subscription& operator=(subscription&& rhs) noexcept
        {
            if (&rhs == this)
                return *this;
            parent = rhs.parent;
            actual = rhs.actual;
            rhs.parent = nullptr;
            return *this;
        }

        ~subscription()
        {
            if (parent == nullptr)
                return;

            parent->detach(actual);
        }

    private:
        event_bus* parent;
        handle actual;
    };

    template <class Event = void, class Handler> subscription subscribe(Handler&& handler)
    {
        return subscription(this, attach<Event>(std::forward<Handler>(handler)));
    }

private:
    template <class T> class concrete_wrapper : public event_wrapper
    {
    public:
        template <class... Args>
        explicit concrete_wrapper(Args&&... args)
        : m_event{ std::forward<Args>(args)... }
        {
        }

        T const& get() const
        {
            return m_event;
        }

    private:
        T m_event;
    };

    handler_map_t m_handlers;
};

/** Hold multiple subscriptions to the event_bus and disconnect them on destruction.
 */
class subscription_list
{
public:
  subscription_list() = default;
  ~subscription_list() = default;
  subscription_list(subscription_list&&) = default;
  subscription_list(subscription_list const&) = delete;
  subscription_list& operator=(subscription_list const&) = delete;
  subscription_list& operator=(subscription_list&&) = default;

  subscription_list& operator+=(event_bus::subscription rhs)
  {
    m_list.push_back(std::move(rhs));
    return *this;
  }

  void clear()
  {
    m_list.clear();
  }

  /** Helper class for more comfortable subscription via a builder-like interface.
   */
  class subscription_proxy
  {
  public:
    subscription_proxy(subscription_list* list, event_bus* parent)
      : m_list(list), m_bus(parent)
    {
    }
    subscription_proxy(subscription_proxy const&) = default;
    subscription_proxy& operator=(subscription_proxy const&) = default;

    template <typename Event, typename Handler>
    subscription_proxy subscribe(Handler&& handler) const
    {
      *m_list += m_bus->subscribe<Event>(std::forward<Handler>(handler));
      return *this;
    };

  private:
    subscription_list* m_list;
    event_bus* m_bus;
  };


  subscription_proxy operator()(event_bus& bus)
  {
    return subscription_proxy(this, &bus);
  }

private:
  std::vector<event_bus::subscription> m_list;
};

}
