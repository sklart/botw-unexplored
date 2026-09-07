#include <switch.h>

#include <string>

#include "SaveLoadDecision.h"

namespace Accounts
{
    using ProfileSelectionStatus = SaveLoadDecision::ProfilePickerStatus;

    struct ProfileSelection
    {
        ProfileSelectionStatus status = ProfileSelectionStatus::Error;
        AccountUid uid = {};
    };

    ProfileSelection RequestProfileSelection();

    std::string GetNickname(AccountUid uid);
}
