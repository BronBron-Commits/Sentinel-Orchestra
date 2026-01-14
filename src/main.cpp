#include "orchestra.h"
#include <iostream>
#include <cstring>
#include <optional>

struct CounterState : sentinel::SystemState {
    uint64_t counter;
};

struct DummySystem : sentinel::ISystem {
    uint64_t counter = 0;
    std::optional<uint64_t> inject_at;

    void step(uint64_t tick) override {
        counter++;
        if (inject_at && tick == *inject_at) {
            counter ^= 0xDEADBEEF;
            std::cout << "[inject] divergence at tick " << tick << "\n";
        }
    }

    uint64_t hash() const override {
        return counter;
    }

    // FIXED: correct override signature
    std::unique_ptr<sentinel::SystemState> save_state() const override {
        auto state = std::make_unique<CounterState>();
        state->counter = counter;
        return state;
    }

    void load_state(const sentinel::SystemState* state) override {
        counter = static_cast<const CounterState*>(state)->counter;
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
    constexpr uint64_t ROLLBACK_TICK = 4;

    sentinel::Orchestra A;
    sentinel::Orchestra B;

    DummySystem sysA;
    DummySystem sysB;
    sysB.inject_at = parse_inject_arg(argc, argv);

    A.registerSystem(&sysA);
    B.registerSystem(&sysB);

    std::cout << "=== Rollback-Capable Lockstep ===\n";

    for (uint64_t tick = 0; tick < MAX_TICKS; ++tick) {
        A.save_snapshot(tick);
        B.save_snapshot(tick);

        sysA.step(tick);
        sysB.step(tick);

        if (sysA.hash() != sysB.hash()) {
            std::cout << "[rollback] restoring to tick "
                      << ROLLBACK_TICK << "\n";
            A.restore_snapshot(ROLLBACK_TICK);
            B.restore_snapshot(ROLLBACK_TICK);
            break;
        }
    }

    return 0;
}
