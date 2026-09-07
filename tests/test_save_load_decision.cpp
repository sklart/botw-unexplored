#include <cassert>

#include "SaveLoadDecision.h"

int main()
{
    using SaveLoadDecision::Resolve;
    using SaveLoadDecision::MountStatus;
    assert(Resolve(MountStatus::Success, false, true).success);
    assert(!Resolve(MountStatus::Success, false, false).success);
    assert(Resolve(MountStatus::SaveInaccessible, false, true).success);
    assert(Resolve(MountStatus::SaveInaccessible, false, true).loadedSavefile);
    assert(!Resolve(MountStatus::SaveInaccessible, false, true).masterModeLoaded);
    assert(Resolve(MountStatus::SaveInaccessible, true, true).success);
    assert(Resolve(MountStatus::SaveInaccessible, true, true).masterModeLoaded);
    assert(!Resolve(MountStatus::SaveInaccessible, false, false).success);
    assert(Resolve(MountStatus::SaveInaccessible, false, false).showGameRunningDialog);
    assert(!Resolve(MountStatus::NotSelected, false, true).success);
    assert(!Resolve(MountStatus::NoSave, false, true).success);
    assert(!Resolve(MountStatus::AccountError, false, true).success);
    assert(SaveLoadDecision::ResolveProfilePicker(true, true) == SaveLoadDecision::ProfilePickerStatus::Selected);
    assert(SaveLoadDecision::ResolveProfilePicker(true, false) == SaveLoadDecision::ProfilePickerStatus::NotSelected);
    assert(SaveLoadDecision::ResolveProfilePicker(false, true) == SaveLoadDecision::ProfilePickerStatus::Error);
    assert(static_cast<int>(MountStatus::NotSelected) != static_cast<int>(MountStatus::NoSave));
}
