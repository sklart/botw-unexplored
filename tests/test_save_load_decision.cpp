#include <cassert>

#include "SaveLoadDecision.h"

int main()
{
    using SaveLoadDecision::Resolve;
    assert(Resolve(1, false, true).success);
    assert(!Resolve(1, false, false).success);
    assert(Resolve(-1, false, true).success);
    assert(Resolve(-1, false, true).loadedSavefile);
    assert(!Resolve(-1, false, true).masterModeLoaded);
    assert(Resolve(-1, true, true).success);
    assert(Resolve(-1, true, true).masterModeLoaded);
    assert(!Resolve(-1, false, false).success);
    assert(Resolve(-1, false, false).showGameRunningDialog);
    assert(!Resolve(0, false, true).success);
    assert(!Resolve(-2, false, true).success);
}
