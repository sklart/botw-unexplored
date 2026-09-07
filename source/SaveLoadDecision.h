#pragma once

namespace SaveLoadDecision
{
    struct Outcome
    {
        bool success = false;
        bool loadedSavefile = false;
        bool masterModeLoaded = false;
        bool showGameRunningDialog = false;
    };

    Outcome Resolve(int mountStatus, bool requestedMasterMode, bool parseSucceeded);
}
