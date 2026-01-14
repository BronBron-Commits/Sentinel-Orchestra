#pragma once

#include <vector>
#include <cstdint>

namespace sentinel {

class ISystem {
public:
    virtual ~ISystem() = default;
    virtual void step(uint64_t tick) = 0;
    virtual uint64_t hash() const = 0;
};

struct Snapshot {
    uint64_t tick;
    uint64_t hash;
};

class Orchestra {
public:
    explicit Orchestra(size_t snapshot_capacity = 16);

    void registerSystem(ISystem* system) {
        systems.push_back(system);
    }

    void run(uint64_t maxTicks);

    // Lockstep execution + divergence detection
    static bool run_lockstep(
        Orchestra& A,
        Orchestra& B,
        uint64_t maxTicks
    );

private:
    void record_snapshot(uint64_t tick, uint64_t hash);
    void dump_snapshots() const;
    uint64_t compute_hash() const;

    std::vector<ISystem*> systems;

    std::vector<Snapshot> snapshots;
    size_t snapshot_capacity;
    size_t snapshot_head;
};

} // namespace sentinel
