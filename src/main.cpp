#include "orchestra.h"
#include <iostream>
#include <cstring>
#include <optional>
#include <memory>

struct CounterState : sentinel::SystemState {
    uint64_t counter;
};

struct DummySystem : sentinel::ISystem {
    uint64_t counter = 0;
    std::optional<uint64_t> inject_at;
    bool replaying = false;

    void set_replay_mode(bool replay) override {
        replaying = replay;
        if (replay)
            inject_at.reset();
    }

    void step(uint64_t tick) override {
        counter++;
        if (!replaying && inject_at && tick == *inject_at) {
            counter ^= 0xDEADBEEF;
            std::cout << "[inject] divergence at tick " << tick << "\n";
        }
    }

    uint64_t hash() const override { return counter; }

    std::unique_ptr<sentinel::SystemState> save_state() const override {
        auto s = std::make_unique<CounterState>();
        s->counter = counter;
        return s;
    }

    void load_state(const sentinel::SystemState* state) override {
        counter = static_cast<const CounterState*>(state)->counter;
    }
};

static std::optional<uint64_t> parse_inject_arg(int argc, char** argv) {
    for (int i = 1; i < argc - 1; ++i)
        if (!std::strcmp(argv[i], "--inject-divergence-at"))
            return std::stoull(argv[i + 1]);
    return std::nullopt;
}

int main(int argc, char** argv) {
    constexpr uint64_t ROLLBACK_WINDOW = 3;

    sentinel::Orchestra A(ROLLBACK_WINDOW);
    sentinel::Orchestra B(ROLLBACK_WINDOW);

    DummySystem sysA;
    DummySystem sysB;
    sysB.inject_at = parse_inject_arg(argc, argv);

    A.registerSystem(&sysA);
    B.registerSystem(&sysB);

    sentinel::Orchestra::run_lockstep_with_rollback(
        A, B, /*maxTicks=*/10
    );

    return 0;
}
