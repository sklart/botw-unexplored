#include <cassert>
#include <cstdint>
#include <vector>

#include "SaveStateTransaction.h"

namespace
{
    struct TestSaveState
    {
        uint64_t uid1 = 0;
        uint64_t uid2 = 0;
        int normalSlot = -1;
        int masterSlot = -1;
        bool loaded = false;
        bool gameRunning = false;
        bool masterLoaded = false;
        bool hasDLC = false;
        std::vector<uint32_t> foundObjects;
    };

    bool operator==(const TestSaveState& left, const TestSaveState& right)
    {
        return left.uid1 == right.uid1 && left.uid2 == right.uid2 &&
               left.normalSlot == right.normalSlot && left.masterSlot == right.masterSlot &&
               left.loaded == right.loaded && left.gameRunning == right.gameRunning &&
               left.masterLoaded == right.masterLoaded && left.hasDLC == right.hasDLC &&
               left.foundObjects == right.foundObjects;
    }
}

int main()
{
    const TestSaveState stateA = {1, 2, 3, 7, true, false, false, true, {10, 20}};
    const TestSaveState stateB = {11, 22, 4, 6, true, true, true, false, {30}};

    TestSaveState live = stateA;
    assert(SaveStateTransaction::Commit(live, stateB, true));
    assert(live == stateB); // Current state A -> valid candidate B -> state B.

    live = stateA;
    assert(!SaveStateTransaction::Commit(live, stateB, false));
    assert(live == stateA); // Parse failure keeps every collection and flag from A.

    live = stateA;
    assert(!SaveStateTransaction::Commit(live, stateB, false));
    assert(live == stateA); // Backup failure has the same no-commit behavior.

    live = stateA;
    assert(!SaveStateTransaction::Commit(live, stateB, false));
    assert(!live.masterLoaded); // Failed Master Mode load leaves the Normal state active.

    live = stateA;
    const TestSaveState failedProfileB = {99, 100, 4, 6, false, false, false, false, {30}};
    assert(!SaveStateTransaction::Commit(live, failedProfileB, false));
    assert(live.uid1 == stateA.uid1 && live.uid2 == stateA.uid2);

    live = stateA;
    const TestSaveState malformedCandidate = {99, 100, -1, -1, false, false, false, false, {}};
    assert(!SaveStateTransaction::Commit(live, malformedCandidate, false));
    assert(live == stateA); // Malformed or truncated input never reaches live state.
}
