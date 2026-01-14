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

    virtual std::unique_ptr<SystemState> save_state() const {
        return nullptr;
    }

    virtual void load_state(const SystemState* /*state*/) {}

    virtual void set_replay_mode(bool /*replay*/) {}
};

struct RollbackSnapshot {
    uint64_t tick;
    std::vector<std::unique_ptr<SystemState>> states;
};

class Orchestra {
public:
    explicit Orchestra(uint64_t rollbackWindow);

    void registerSystem(ISystem* system) {
        systems.push_back(system);
    }

    static bool run_lockstep_with_rollback(
        Orchestra& A,
        Orchestra& B,
        uint64_t maxTicks
    );

private:
    uint64_t compute_hash() const;

    void save_snapshot(uint64_t tick);
    bool restore_snapshot(uint64_t tick);
    void evict_old_snapshots(uint64_t currentTick);

    void set_replay_mode(bool replay);

    uint64_t rollbackWindow;
    std::vector<ISystem*> systems;
    std::unordered_map<uint64_t, RollbackSnapshot> rollback_snapshots;
};

} // namespace sentinel
