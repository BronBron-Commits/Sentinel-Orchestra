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

void Orchestra::record_snapshot(uint64_t tick, uint64_t hash) {
    snapshots[snapshot_head] = {tick, hash};
    snapshot_head = (snapshot_head + 1) % snapshot_capacity;
}

void Orchestra::dump_snapshots() const {
    std::cout << "\n=== Snapshot Ring Buffer (most recent last) ===\n";

    for (size_t i = 0; i < snapshot_capacity; ++i) {
        const Snapshot& s =
            snapshots[(snapshot_head + i) % snapshot_capacity];

        if (s.tick == 0 && s.hash == 0)
            continue;

        std::cout
            << "tick " << std::setw(3) << s.tick
            << "  hash=0x" << std::hex << s.hash << std::dec
            << "\n";
    }
}

void Orchestra::run(uint64_t maxTicks) {
    for (uint64_t tick = 0; tick < maxTicks; ++tick) {
        for (auto* system : systems) {
            system->step(tick);
        }

        uint64_t hash = 14695981039346656037ull;
        for (auto* system : systems) {
            hash = combine_hash(hash, system->hash());
        }

        record_snapshot(tick, hash);

        std::cout
            << "[tick " << tick << "] hash=0x"
            << std::hex << hash << std::dec << "\n";
    }

    dump_snapshots();
}

} // namespace sentinel
