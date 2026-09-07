#include "Accounts.h"

#include "Log.h"

Accounts::ProfileSelection Accounts::RequestProfileSelection()
{
    Log("Requesting profile picker...");

    ProfileSelection selection;
    PselUserSelectionSettings settings = {};
    AccountUid selectedUid = {};
    const Result result = pselShowUserSelector(&selectedUid, &settings);
    selection.status = SaveLoadDecision::ResolveProfilePicker(R_SUCCEEDED(result), accountUidIsValid(&selectedUid));
    if (selection.status != ProfileSelectionStatus::Selected)
    {
        Log(selection.status == ProfileSelectionStatus::NotSelected ? "PlayerSelect returned no user" : "PlayerSelect failed");
        return selection;
    }
    selection.uid = selectedUid;
    return selection;
}
