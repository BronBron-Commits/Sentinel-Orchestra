#include "orchestra.h"
#include "simcore_system.h"
#include <iostream>

int main() {
    constexpr uint64_t ROLLBACK_WINDOW = 3;

    sentinel::Orchestra A(ROLLBACK_WINDOW);
    sentinel::Orchestra B(ROLLBACK_WINDOW);

    SimCoreSystem sysA;
    SimCoreSystem sysB;

    A.registerSystem(&sysA);
    B.registerSystem(&sysB);

    sentinel::Orchestra::run_lockstep_with_rollback(
        A, B, /*maxTicks=*/20
    );

    return 0;
}
