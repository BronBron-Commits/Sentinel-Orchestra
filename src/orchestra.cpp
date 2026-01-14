#include "orchestra.h"
#include <iostream>

namespace sentinel {

static uint64_t combine_hash(uint64_t h, uint64_t v) {
    constexpr uint64_t FNV_PRIME = 1099511628211ull;
    h ^= v;
    h *= FNV_PRIME;
    return h;
}

Orchestra::Orchestra(uint64_t window)
    : rollbackWindow(window) {}

void Orchestra::registerSystem(ISystem* system) {
    systems.push_back(system);
}

void Orchestra::inject_input(uint64_t tick, int channel, int64_t value) {
    input_journal[tick].push_back({tick, channel, value});
}

void Orchestra::apply_inputs(uint64_t tick) {
    auto it = input_journal.find(tick);
    if (it == input_journal.end())
        return;

    // deterministic input hook — systems may read shared input state
    (void)it;
}

uint64_t Orchestra::compute_hash() const {
    uint64_t h = 14695981039346656037ull;
    for (auto* s : systems)
        h = combine_hash(h, s->hash());
    return h;
}

void Orchestra::set_replay_mode(bool replay) {
    for (auto* s : systems)
        s->set_replay_mode(replay);
}

void Orchestra::evict_old_snapshots(uint64_t currentTick) {
    if (currentTick < rollbackWindow)
        return;

    uint64_t minTick = currentTick - rollbackWindow;

    for (auto it = rollback_snapshots.begin();
         it != rollback_snapshots.end(); ) {
        if (it->first < minTick)
            it = rollback_snapshots.erase(it);
        else
            ++it;
    }
}

void Orchestra::save_snapshot(uint64_t tick) {
    RollbackSnapshot snap;
    snap.tick = tick;

    for (auto* s : systems)
        snap.states.push_back(s->save_state());

    rollback_snapshots[tick] = std::move(snap);
    evict_old_snapshots(tick);
}

bool Orchestra::restore_snapshot(uint64_t tick) {
    auto it = rollback_snapshots.find(tick);
    if (it == rollback_snapshots.end())
        return false;

    const RollbackSnapshot& snap = it->second;
    for (size_t i = 0; i < systems.size(); ++i)
        systems[i]->load_state(snap.states[i].get());

    return true;
}

bool Orchestra::run_lockstep_with_rollback(
    Orchestra& A,
    Orchestra& B,
    uint64_t maxTicks
) {
    std::cout << "=== Lockstep + Rollback + Replay ===\n";

    uint64_t divergenceTick = UINT64_MAX;

    for (uint64_t t = 0; t < maxTicks; ++t) {
        A.save_snapshot(t);
        B.save_snapshot(t);

        A.apply_inputs(t);
        B.apply_inputs(t);

        for (auto* s : A.systems) s->step(t);
        for (auto* s : B.systems) s->step(t);

        uint64_t hA = A.compute_hash();
        uint64_t hB = B.compute_hash();

        std::cout << "[tick " << t << "] A=0x"
                  << std::hex << hA
                  << " B=0x" << hB << std::dec;

        if (hA == hB) {
            std::cout << " OK\n";
        } else {
            std::cout << " MISMATCH\n";
            divergenceTick = t;
            break;
        }
    }

    if (divergenceTick == UINT64_MAX)
        return true;

    uint64_t rollbackTick =
        (divergenceTick > A.rollbackWindow)
        ? divergenceTick - A.rollbackWindow
        : 0;

    std::cout << "[rollback] restoring to tick "
              << rollbackTick << "\n";

    if (!A.restore_snapshot(rollbackTick) ||
        !B.restore_snapshot(rollbackTick)) {
        std::cout << "[rollback] snapshot unavailable\n";
        return false;
    }

    A.set_replay_mode(true);
    B.set_replay_mode(true);

    std::cout << "[replay] replaying ticks "
              << rollbackTick + 1
              << " → " << divergenceTick << "\n";

    for (uint64_t t = rollbackTick + 1; t <= divergenceTick; ++t) {
        A.apply_inputs(t);
        B.apply_inputs(t);

        for (auto* s : A.systems) s->step(t);
        for (auto* s : B.systems) s->step(t);

        if (A.compute_hash() != B.compute_hash()) {
            std::cout << "[replay tick " << t << "] STILL DIVERGED\n";
            return false;
        }
    }

    A.set_replay_mode(false);
    B.set_replay_mode(false);

    std::cout << "[replay] convergence successful\n";
    return true;
}

} // namespace sentinel
