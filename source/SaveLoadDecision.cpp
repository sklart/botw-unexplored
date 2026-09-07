#include "SaveLoadDecision.h"

SaveLoadDecision::Outcome SaveLoadDecision::Resolve(MountStatus mountStatus, bool requestedMasterMode, bool parseSucceeded)
{
    Outcome outcome;
    if (mountStatus == MountStatus::Success || mountStatus == MountStatus::SaveInaccessible)
    {
        outcome.success = parseSucceeded;
        outcome.loadedSavefile = parseSucceeded;
        outcome.masterModeLoaded = parseSucceeded && requestedMasterMode;
        outcome.showGameRunningDialog = mountStatus == MountStatus::SaveInaccessible && !parseSucceeded;
    }
    return outcome;
}

SaveLoadDecision::ProfilePickerStatus SaveLoadDecision::ResolveProfilePicker(bool transportSucceeded,
                                                                               unsigned long long appletResult,
                                                                               bool validUid)
{
    if (!transportSucceeded || !validUid)
        return ProfilePickerStatus::Error;
    return appletResult == 0 ? ProfilePickerStatus::Selected : ProfilePickerStatus::Cancelled;
}
