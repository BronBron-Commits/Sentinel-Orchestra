#include "simcore_system.h"

/* sim-core API is GLOBAL */
SimCoreSystem::SimCoreSystem()
    : state(sim_initial_state()) {}

void SimCoreSystem::step(uint64_t /*tick*/) {
    sim_update(state);
}

uint64_t SimCoreSystem::hash() const {
    return sim_hash(state);
}

std::unique_ptr<sentinel::SystemState>
SimCoreSystem::save_state() const {
    auto s = std::make_unique<SimCoreState>();
    s->state = state;
    return s;
}

void SimCoreSystem::load_state(const sentinel::SystemState* s) {
    state = static_cast<const SimCoreState*>(s)->state;
}
