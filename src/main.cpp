#include "orchestra.h"
#include <iostream>

struct DummySystem : sentinel::ISystem {
    uint64_t counter = 0;

    void step(uint64_t) override {
        counter++;
    }

    uint64_t hash() const override {
        return counter;
    }
};

int main() {
    sentinel::Orchestra orchestra;
    DummySystem dummy;

    orchestra.registerSystem(&dummy);
    orchestra.run(10);

    return 0;
}
