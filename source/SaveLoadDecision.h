#pragma once

namespace SaveLoadDecision
{
    enum class MountStatus
    {
        Success = 1,
        NotSelected = 0,
        SaveInaccessible = -1,
        NoSave = -2,
        AccountError = -3
    };

    enum class ProfilePickerStatus
    {
        Selected,
        NotSelected,
        Error
    };

    struct Outcome
    {
        bool success = false;
        bool loadedSavefile = false;
        bool masterModeLoaded = false;
        bool showGameRunningDialog = false;
    };

    Outcome Resolve(MountStatus mountStatus, bool requestedMasterMode, bool parseSucceeded);
    ProfilePickerStatus ResolveProfilePicker(bool transportSucceeded, bool validUid);
}
