#include "orchestra.h"
#include <iostream>

namespace sentinel {

static uint64_t combine_hash(uint64_t h, uint64_t v) {
    constexpr uint64_t FNV_OFFSET = 14695981039346656037ull;
    constexpr uint64_t FNV_PRIME  = 1099511628211ull;
    h ^= v;
    h *= FNV_PRIME;
    return h;
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

        std::cout
            << "[tick " << tick << "] hash=0x"
            << std::hex << hash << std::dec << "\n";
    }
}

} // namespace sentinel
