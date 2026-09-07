#include "Accounts.h"

#include <string.h>
#include <stdio.h>
#include "Log.h"

Accounts::ProfileSelection Accounts::RequestProfileSelection()
{
    Log("Requesting profile picker...");

    struct UserReturnData
    {
        u64 result;
        AccountUid UID;
    };

    ProfileSelection selection;
    UserReturnData outdata = {};
    AppletHolder holder = {};
    AppletStorage outputStorage = {};
    AppletStorage inputStorage = {};
    LibAppletArgs args = {};
    bool holderCreated = false;
    bool inputStorageCreated = false;
    bool outputStorageCreated = false;

    u8 indata[0xA0] = { 0 };
    indata[0x96] = 1;

    Result res = appletCreateLibraryApplet(&holder, AppletId_LibraryAppletPlayerSelect, LibAppletMode_AllForeground);
    if (R_FAILED(res))
    {
        Log("appletCreateLibraryApplet() failed");
        return selection;
    }
    holderCreated = true;
    libappletArgsCreate(&args, 0);
    res = libappletArgsPush(&args, &holder);
    if (R_FAILED(res))
    {
        Log("libappletArgsPush() failed");
        appletHolderClose(&holder);
        return selection;
    }

    res = appletCreateStorage(&inputStorage, sizeof(indata));
    if (R_FAILED(res))
    {
        Log("appletCreateStorage() failed");
        appletHolderClose(&holder);
        return selection;
    }
    inputStorageCreated = true;

    res = appletStorageWrite(&inputStorage, 0, indata, sizeof(indata));
    if (R_SUCCEEDED(res))
        res = appletHolderPushInData(&holder, &inputStorage);
    if (R_SUCCEEDED(res))
        res = appletHolderStart(&holder);
    if (R_FAILED(res))
    {
        Log("PlayerSelect applet setup failed");
        appletStorageClose(&inputStorage);
        appletHolderClose(&holder);
        return selection;
    }

    while (appletHolderWaitInteractiveOut(&holder)) {}

    appletHolderJoin(&holder);
    res = appletHolderPopOutData(&holder, &outputStorage);
    if (R_SUCCEEDED(res))
    {
        outputStorageCreated = true;
        res = appletStorageRead(&outputStorage, 0, &outdata, sizeof(outdata));
    }

    if (outputStorageCreated)
        appletStorageClose(&outputStorage);
    if (inputStorageCreated)
        appletStorageClose(&inputStorage);
    if (holderCreated)
        appletHolderClose(&holder);

    selection.status = SaveLoadDecision::ResolveProfilePicker(R_SUCCEEDED(res), outdata.result,
                                                               accountUidIsValid(&outdata.UID));
    if (selection.status != ProfileSelectionStatus::Selected)
    {
        Log(selection.status == ProfileSelectionStatus::Cancelled ? "PlayerSelect was cancelled" : "PlayerSelect applet result read failed");
        return selection;
    }
    selection.uid = outdata.UID;
    return selection;
}

// doesn't work
std::string Accounts::GetNickname(AccountUid uid)
{
    char nickname[0x21];

    AccountProfile profile;
    AccountProfileBase profileBase;

    memset(&profileBase, 0, sizeof(profileBase));

    Result rc = accountGetProfile(&profile, uid);
    if (R_FAILED(rc))
        printf("accountGetProfile() failed\n");

    rc = accountProfileGet(&profile, NULL, &profileBase);
    if (R_FAILED(rc))
        printf("accountProfileGet() failed\n");

    memset(nickname,  0, sizeof(nickname));
    strncpy(nickname, profileBase.nickname, sizeof(nickname) - 1);//Copy the nickname elsewhere to make sure it's NUL-terminated.

    return std::string(nickname);
}
