#include "orchestra/orchestra.hpp"

namespace orchestra {

Orchestrator::Orchestrator() {
    TimelineState root;
    root.current = sim_initial_state();
    root.snapshots[0] = root.current;
    root.tick = 0;
    timelines_[0] = root;
}

Timeline Orchestrator::create_timeline() {
    Timeline id = next_id_++;
    timelines_[id] = timelines_[0];
    return id;
}

Timeline Orchestrator::fork(Timeline parent, uint32_t tick) {
    auto it = timelines_.find(parent);
    if (it == timelines_.end())
        return 0;

    auto snap = it->second.snapshots.find(tick);
    if (snap == it->second.snapshots.end())
        return 0;

    Timeline id = next_id_++;
    TimelineState child;
    child.current = snap->second;
    child.snapshots[tick] = child.current;
    child.tick = tick;

    timelines_[id] = child;
    return id;
}

void Orchestrator::inject_input(Timeline tl, uint32_t tick, int channel, double value) {
    auto it = timelines_.find(tl);
    if (it == timelines_.end())
        return;

    it->second.inputs.push_back({tick, channel, value});
}

void Orchestrator::run(const ScenarioConfig& cfg) {
    for (uint32_t step = 0; step < cfg.max_ticks; ++step) {
        for (auto& [id, tl] : timelines_) {

            // Apply inputs scheduled for this tick (no-op hook for now)
            for (const auto& ev : tl.inputs) {
                if (ev.tick == tl.tick) {
                    // Placeholder: deterministic input handling point
                    (void)ev;
                }
            }

            sim_update(tl.current);
            tl.tick++;
            tl.snapshots[tl.tick] = tl.current;
        }
    }
}

uint64_t Orchestrator::hash(Timeline tl) const {
    auto it = timelines_.find(tl);
    if (it == timelines_.end())
        return 0;
    return sim_hash(it->second.current);
}

} // namespace orchestra
