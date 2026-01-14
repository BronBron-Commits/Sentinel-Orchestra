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

class Orchestra {
public:
    Orchestra() = default;

    void registerSystem(ISystem* system) {
        systems.push_back(system);
    }

    void run(uint64_t maxTicks);

private:
    std::vector<ISystem*> systems;
};

} // namespace sentinel
