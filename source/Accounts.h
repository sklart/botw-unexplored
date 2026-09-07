#pragma once

#include <switch.h>

#include "SaveLoadDecision.h"

namespace Accounts
{
    using ProfileSelectionStatus = SaveLoadDecision::ProfilePickerStatus;

    struct ProfileSelection
    {
        ProfileSelectionStatus status = ProfileSelectionStatus::Error;
        AccountUid uid = {};
    };

    // The caller must keep accountInitialize() active for this invocation.
    ProfileSelection RequestProfileSelection();
}
