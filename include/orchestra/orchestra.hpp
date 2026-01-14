#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <vector>

#include <simcore/simcore.hpp>

namespace orchestra {

using Timeline = uint32_t;

struct ScenarioConfig {
    uint32_t max_ticks = 0;
};

struct InputEvent {
    uint32_t tick;
    int channel;
    double value;
};

class Orchestrator {
public:
    Orchestrator();

    Timeline create_timeline();
    Timeline fork(Timeline parent, uint32_t tick);

    void inject_input(Timeline tl, uint32_t tick, int channel, double value);

    void run(const ScenarioConfig& cfg);
    uint64_t hash(Timeline tl) const;

private:
    struct TimelineState {
        SimState current;
        std::map<uint32_t, SimState> snapshots;
        std::vector<InputEvent> inputs;
        uint32_t tick = 0;
    };

    std::unordered_map<Timeline, TimelineState> timelines_;
    Timeline next_id_ = 1;
};

} // namespace orchestra
