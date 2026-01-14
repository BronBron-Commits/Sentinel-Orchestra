#include "orchestra.h"
#include <iostream>
#include <cstring>
#include <optional>

struct DummySystem : sentinel::ISystem {
    uint64_t counter = 0;

    void step(uint64_t) override {
        counter++;
    }

    uint64_t hash() const override {
        return counter;
    }
};

static std::optional<uint64_t> parse_inject(int argc, char** argv) {
    for (int i = 1; i < argc - 1; ++i)
        if (!std::strcmp(argv[i], "--inject-at"))
            return std::stoull(argv[i + 1]);
    return std::nullopt;
}

int main(int argc, char** argv) {
    sentinel::Orchestra A(3);
    sentinel::Orchestra B(3);

    DummySystem a;
    DummySystem b;

    A.registerSystem(&a);
    B.registerSystem(&b);

    if (auto tick = parse_inject(argc, argv)) {
        A.inject_input(*tick, 0, 1);
        B.inject_input(*tick, 0, 1);
    }

    sentinel::Orchestra::run_lockstep_with_rollback(A, B, 10);
}
