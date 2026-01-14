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
    constexpr uint64_t MAX_TICKS = 10;

    sentinel::Orchestra orchestraA;
    sentinel::Orchestra orchestraB;

    DummySystem sysA;
    DummySystem sysB;

    // Only B gets divergence injection
    sysB.inject_at = parse_inject_arg(argc, argv);

    orchestraA.registerSystem(&sysA);
    orchestraB.registerSystem(&sysB);

    std::cout << "=== Lockstep Orchestra Demo ===\n";

    for (uint64_t tick = 0; tick < MAX_TICKS; ++tick) {
        sysA.step(tick);
        sysB.step(tick);

        uint64_t hashA = sysA.hash();
        uint64_t hashB = sysB.hash();

        std::cout
            << "[tick " << tick << "] "
            << "A=0x" << std::hex << hashA
            << "  B=0x" << hashB << std::dec;

        if (hashA == hashB) {
            std::cout << "  OK\n";
        } else {
            std::cout << "  MISMATCH\n";
        }
    }

    return 0;
}
