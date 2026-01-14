#pragma once

#include "orchestra.h"
#include <simcore/simcore.hpp>
#include <memory>

/*
 * Adapter: Sentinel-Orchestra ↔ Sentinel-Sim-Core
 * sim-core symbols are GLOBAL (not namespaced)
 */

struct SimCoreState : sentinel::SystemState {
    SimState state;
};

class SimCoreSystem : public sentinel::ISystem {
public:
    SimCoreSystem();

    void step(uint64_t tick) override;
    uint64_t hash() const override;

    std::unique_ptr<sentinel::SystemState> save_state() const override;
    void load_state(const sentinel::SystemState* state) override;

private:
    SimState state;
};
