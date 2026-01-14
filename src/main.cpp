#include "orchestra.h"
#include <iostream>
#include <cstring>
#include <optional>

struct DummySystem : sentinel::ISystem {
    uint64_t counter = 0;
    std::optional<uint64_t> inject_at;

    void step(uint64_t tick) override {
        counter++;

        if (inject_at && tick == *inject_at) {
            // Intentional deterministic corruption
            counter ^= 0xDEADBEEF;
            std::cout << "[inject] divergence injected at tick " << tick << "\n";
        }
    }

    uint64_t hash() const override {
        return counter;
    }
};

static std::optional<uint64_t> parse_inject_arg(int argc, char** argv) {
    for (int i = 1; i < argc - 1; ++i) {
        if (std::strcmp(argv[i], "--inject-divergence-at") == 0) {
            return std::stoull(argv[i + 1]);
        }
    }
    return std::nullopt;
}

int main(int argc, char** argv) {
    sentinel::Orchestra orchestra;
    DummySystem dummy;

    dummy.inject_at = parse_inject_arg(argc, argv);

    orchestra.registerSystem(&dummy);
    orchestra.run(10);

    return 0;
}
