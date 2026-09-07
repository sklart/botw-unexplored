#include "SaveLoadDecision.h"

SaveLoadDecision::Outcome SaveLoadDecision::Resolve(int mountStatus, bool requestedMasterMode, bool parseSucceeded)
{
    Outcome outcome;
    if (mountStatus == 1 || mountStatus == -1)
    {
        outcome.success = parseSucceeded;
        outcome.loadedSavefile = parseSucceeded;
        outcome.masterModeLoaded = parseSucceeded && requestedMasterMode;
        outcome.showGameRunningDialog = mountStatus == -1 && !parseSucceeded;
    }
    return outcome;
}
