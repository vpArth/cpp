#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <list>
#include <algorithm>

namespace arth::lib {
  class EventEmitter {
    template<typename T> struct TypeCast {
      typedef T type;
    };
  public:
    EventEmitter();
    EventEmitter(const EventEmitter &) = delete;
    const EventEmitter &operator=(const EventEmitter &) = delete;
    ~EventEmitter();

    template<typename... Args>
    unsigned int on(unsigned int event_id, typename TypeCast<std::function<void(Args...)> >::type cb);

    template<typename... Args>
    void emit(unsigned int event_id, Args... args);
    void off(unsigned int listener_id);

  private:
    struct IListener {
      IListener() : id(0) {}
      explicit IListener(unsigned int i) : id(i) {}
      virtual ~IListener() = default;
      unsigned int id;
    };

    template<typename... Args>
    struct Listener : public IListener {
      Listener() = default;
      Listener(unsigned int i, std::function<void(Args...)> cb) : IListener(i), cb(std::move(cb)) {}

      std::function<void(Args...)> cb;
    };

    std::mutex mutex;
    unsigned int last_listener{};
    std::multimap<unsigned int, std::shared_ptr<IListener>> listeners;
    std::unordered_map<int, std::list<std::shared_ptr<IListener>>> _listeners;
  };

  template<typename... Args>
  unsigned int EventEmitter::on(unsigned int event_id, typename TypeCast<std::function<void(Args...)> >::type cb) {
    if (!cb) {
      throw std::invalid_argument("EventEmitter::add_listener: No callback provided.");
    }

    std::lock_guard<std::mutex> lock(mutex);

    unsigned int listener_id = ++last_listener;
    listeners.insert(std::make_pair(event_id, std::make_shared<Listener<Args...>>(listener_id, cb)));

    return listener_id;
  }

  template<typename... Args>
  void EventEmitter::emit(unsigned int event_id, Args... args) {

    std::list<std::shared_ptr<Listener<Args...>>> handlers;

    {
      std::lock_guard<std::mutex> lock(mutex);

      auto range = listeners.equal_range(event_id);
      handlers.resize(static_cast<unsigned long>(std::distance(range.first, range.second)));
      std::transform(
          range.first,
          range.second,
          handlers.begin(),
          [](std::pair<const unsigned int, std::shared_ptr<IListener>> p) {
            auto l = std::dynamic_pointer_cast<Listener<Args...>>(p.second);
            if (l) return l;

            throw std::logic_error("EventEmitter::emit: Invalid event signature.");
          }
      );
    }

    for (auto &h : handlers) {
      h->cb(args...);
    }
  }

}

