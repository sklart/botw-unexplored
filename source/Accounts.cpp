#include "Accounts.h"

#include "Log.h"

Accounts::ProfileSelection Accounts::RequestProfileSelection()
{
    Log("Requesting profile picker...");

    ProfileSelection selection;
    PselUserSelectionSettings settings = {};
    AccountUid selectedUid = {};
    const Result result = pselShowUserSelector(&selectedUid, &settings);
    // The standard selector does not expose a portable cancel Result. A successful
    // call with an invalid UID is therefore treated as an error, not as cancellation.
    selection.status = SaveLoadDecision::ResolveProfilePicker(R_SUCCEEDED(result), false,
                                                               accountUidIsValid(&selectedUid));
    if (selection.status != ProfileSelectionStatus::Selected)
    {
        Log("PlayerSelect failed or returned no user");
        return selection;
    }
    selection.uid = selectedUid;
    return selection;
}
