#include <iostream>
#include "src/ee/EventEmitter.h"

class SomeEE : public arth::lib::EventEmitter {
public:
  static const unsigned int OPEN = 0;
  static const unsigned int ARGS = 1;
};

int main() {
  SomeEE ee;
  auto onOpen = []() { std::cout << "var: open" << std::endl; };
  auto id = ee.on(SomeEE::OPEN, onOpen);
  ee.on(SomeEE::OPEN, []() { std::cout << "lambda: open" << std::endl; });

  ee.emit(SomeEE::OPEN); // both

  ee.off(id); // off var sub
  ee.emit(SomeEE::OPEN); // lambda only

  ee.on<>(SomeEE::ARGS, std::function<void(int, int)>([](int i, int j) {
    std::cout << "Argument: " << i << std::endl;
  }));
  ee.emit<>(SomeEE::ARGS, 42, 4);
  ee.emit<>(SomeEE::ARGS, 24, 0);

  return 0;
}
