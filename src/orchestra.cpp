#include "orchestra.h"
#include <iostream>
#include <iomanip>

namespace sentinel {

static uint64_t combine_hash(uint64_t h, uint64_t v) {
    constexpr uint64_t FNV_OFFSET = 14695981039346656037ull;
    constexpr uint64_t FNV_PRIME  = 1099511628211ull;
    h ^= v;
    h *= FNV_PRIME;
    return h;
}

Orchestra::Orchestra(size_t capacity)
    : snapshot_capacity(capacity), snapshot_head(0) {
    snapshots.resize(snapshot_capacity);
}

uint64_t Orchestra::compute_hash() const {
    uint64_t h = 14695981039346656037ull;
    for (auto* system : systems) {
        h = combine_hash(h, system->hash());
    }
    return h;
}

void Orchestra::record_snapshot(uint64_t tick, uint64_t hash) {
    snapshots[snapshot_head] = {tick, hash};
    snapshot_head = (snapshot_head + 1) % snapshot_capacity;
}

void Orchestra::dump_snapshots() const {
    std::cout << "\n=== Snapshot Ring Buffer ===\n";
    for (size_t i = 0; i < snapshot_capacity; ++i) {
        const Snapshot& s =
            snapshots[(snapshot_head + i) % snapshot_capacity];
        if (s.tick == 0 && s.hash == 0) continue;
        std::cout << "tick " << s.tick
                  << " hash=0x" << std::hex << s.hash << std::dec << "\n";
    }
}

void Orchestra::save_snapshot(uint64_t tick) {
    RollbackSnapshot snap;
    snap.tick = tick;

    snap.states.reserve(systems.size());
    for (auto* system : systems) {
        snap.states.push_back(system->save_state());
    }

    rollback_snapshots[tick] = std::move(snap);
}

bool Orchestra::restore_snapshot(uint64_t tick) {
    auto it = rollback_snapshots.find(tick);
    if (it == rollback_snapshots.end())
        return false;

    const RollbackSnapshot& snap = it->second;

    for (size_t i = 0; i < systems.size(); ++i) {
        systems[i]->load_state(snap.states[i].get());
    }

    return true;
}

void Orchestra::run(uint64_t maxTicks) {
    for (uint64_t tick = 0; tick < maxTicks; ++tick) {
        for (auto* system : systems) {
            system->step(tick);
        }

        uint64_t hash = compute_hash();
        record_snapshot(tick, hash);

        std::cout << "[tick " << tick << "] hash=0x"
                  << std::hex << hash << std::dec << "\n";
    }

    dump_snapshots();
}

bool Orchestra::run_lockstep(
    Orchestra& A,
    Orchestra& B,
    uint64_t maxTicks
) {
    std::cout << "=== Lockstep Orchestra ===\n";

    for (uint64_t tick = 0; tick < maxTicks; ++tick) {
        A.save_snapshot(tick);
        B.save_snapshot(tick);

        for (auto* sys : A.systems) sys->step(tick);
        for (auto* sys : B.systems) sys->step(tick);

        uint64_t hashA = A.compute_hash();
        uint64_t hashB = B.compute_hash();

        std::cout << "[tick " << tick << "] "
                  << "A=0x" << std::hex << hashA
                  << " B=0x" << hashB << std::dec;

        if (hashA != hashB) {
            std::cout << " MISMATCH\n";
            return false;
        }

        std::cout << " OK\n";
    }

    return true;
}

} // namespace sentinel
