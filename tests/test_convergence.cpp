#include <orchestra/orchestra.hpp>
#include <cassert>

using namespace orchestra;

int main() {
    Orchestrator orch;

    Timeline a = orch.create_timeline();
    Timeline b = orch.create_timeline();

    ScenarioConfig cfg;
    cfg.max_ticks = 100;

    orch.inject_input(a, 10, 1,  1.0);
    orch.inject_input(b, 10, 1, -1.0);

    orch.run(cfg);

    uint64_t ha = orch.hash(a);
    uint64_t hb = orch.hash(b);

    assert(ha == hb);
    return 0;
}
