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
    assert(!Resolve(MountStatus::Cancelled, false, true).success);
    assert(!Resolve(MountStatus::NoSave, false, true).success);
    assert(!Resolve(MountStatus::AccountError, false, true).success);
    assert(SaveLoadDecision::ResolveProfilePicker(true, false, true) == SaveLoadDecision::ProfilePickerStatus::Selected);
    assert(SaveLoadDecision::ResolveProfilePicker(true, true, true) == SaveLoadDecision::ProfilePickerStatus::Cancelled);
    assert(SaveLoadDecision::ResolveProfilePicker(false, false, true) == SaveLoadDecision::ProfilePickerStatus::Error);
    assert(SaveLoadDecision::ResolveProfilePicker(true, false, false) == SaveLoadDecision::ProfilePickerStatus::Error);
}
