#pragma once

#include <vector>
#include <cstdint>
#include <unordered_map>
#include <memory>

namespace sentinel {

struct SystemState {
    virtual ~SystemState() = default;
};

class ISystem {
public:
    virtual ~ISystem() = default;

    virtual void step(uint64_t tick) = 0;
    virtual uint64_t hash() const = 0;

    // Rollback hooks
    virtual std::unique_ptr<SystemState> save_state() const {
        return nullptr;
    }

    virtual void load_state(const SystemState* /*state*/) {}
};

struct Snapshot {
    uint64_t tick;
    uint64_t hash;
};

struct RollbackSnapshot {
    uint64_t tick;
    std::vector<std::unique_ptr<SystemState>> states;
};

class Orchestra {
public:
    explicit Orchestra(size_t snapshot_capacity = 16);

    void registerSystem(ISystem* system) {
        systems.push_back(system);
    }

    void run(uint64_t maxTicks);

    static bool run_lockstep(
        Orchestra& A,
        Orchestra& B,
        uint64_t maxTicks
    );

    // Rollback
    void save_snapshot(uint64_t tick);
    bool restore_snapshot(uint64_t tick);

private:
    uint64_t compute_hash() const;
    void record_snapshot(uint64_t tick, uint64_t hash);
    void dump_snapshots() const;

    std::vector<ISystem*> systems;

    std::vector<Snapshot> snapshots;
    size_t snapshot_capacity;
    size_t snapshot_head;

    std::unordered_map<uint64_t, RollbackSnapshot> rollback_snapshots;
};

} // namespace sentinel
