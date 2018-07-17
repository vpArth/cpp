#include "EventEmitter.h"

namespace arth::lib {

  EventEmitter::EventEmitter() = default;
  EventEmitter::~EventEmitter() = default;

  void EventEmitter::off(unsigned int listener_id) {
    std::lock_guard<std::mutex> lock(mutex);

    auto i = std::find_if(listeners.begin(), listeners.end(),
                          [&](std::pair<const unsigned int, std::shared_ptr<IListener>> p) {
                            return p.second->id == listener_id;
                          });
    if (i != listeners.end()) {
      listeners.erase(i);
    } else {
      throw std::invalid_argument("EventEmitter::off: Invalid listener id.");
    }
  }

}