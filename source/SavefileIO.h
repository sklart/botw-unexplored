#pragma once

#include <vector>

#include "SaveLoadDecision.h"
#include <cstdint>
#include <switch.h>
#include <thread>

#include "Data.h"

namespace SavefileIO
{
    struct ParsedSaveState
    {
        bool hasDLC = false;
        std::vector<Data::Korok*> foundKoroks;
        std::vector<Data::Korok*> missingKoroks;
        std::vector<Data::Shrine*> foundShrines;
        std::vector<Data::Shrine*> missingShrines;
        std::vector<Data::DLCShrine*> foundDLCShrines;
        std::vector<Data::DLCShrine*> missingDLCShrines;
        std::vector<Data::Location*> visitedLocations;
        std::vector<Data::Location*> unexploredLocations;
        std::vector<Data::Hinox*> defeatedHinoxes;
        std::vector<Data::Hinox*> undefeatedHinoxes;
        std::vector<Data::Talus*> defeatedTaluses;
        std::vector<Data::Talus*> undefeatedTaluses;
        std::vector<Data::Molduga*> defeatedMoldugas;
        std::vector<Data::Molduga*> undefeatedMoldugas;
    };

    struct SaveState : ParsedSaveState
    {
        u64 accountUid1 = 0;
        u64 accountUid2 = 0;
        int mostRecentNormalModeFile = -1;
        int mostRecentMasterModeFile = -1;
        int masterModeSlot = 0;
        bool loadedSavefile = false;
        bool gameIsRunning = false;
        bool noSavefileForUser = false;
        bool masterModeFileExists = false;
        bool masterModeFileLoaded = false;
    };

    extern SaveState CurrentState;

    extern std::vector<Data::Korok*>& foundKoroks;
    extern std::vector<Data::Korok*>& missingKoroks;
    extern std::vector<Data::Shrine*>& foundShrines;
    extern std::vector<Data::Shrine*>& missingShrines;
    extern std::vector<Data::DLCShrine*>& foundDLCShrines;
    extern std::vector<Data::DLCShrine*>& missingDLCShrines;
    extern std::vector<Data::Location*>& visitedLocations;
    extern std::vector<Data::Location*>& unexploredLocations;
    extern std::vector<Data::Hinox*>& defeatedHinoxes;
    extern std::vector<Data::Hinox*>& undefeatedHinoxes;
    extern std::vector<Data::Talus*>& defeatedTaluses;
    extern std::vector<Data::Talus*>& undefeatedTaluses;
    extern std::vector<Data::Molduga*>& defeatedMoldugas;
    extern std::vector<Data::Molduga*>& undefeatedMoldugas;

    bool LoadGamesave(bool loadMasterMode = false, bool chooseProfile = false);

    uint32_t ReadU32(const uint8_t* buffer, size_t offset);

    SaveLoadDecision::MountStatus MountSavefile(bool openProfilePicker, SaveState& candidate);
    bool UnmountSavefile();

    bool LoadBackup(bool masterMode, SaveState& candidate);

    uint32_t GetSavefilePlaytime(const std::string& filepath);
    int GetMostRecentSavefile(const std::string& dir, bool masterMode = false); // Needs slash at end of dir

    void CopySavefiles();
    s32 CopyFile(const std::string &srcPath, const std::string &dstPath);

    bool ParseFile(const char* filepath);

    bool DirectoryExists(const std::string& filepath);
    bool FileExists(const std::string& filepath);

    extern u64& AccountUid1;
    extern u64& AccountUid2;

    extern int& MostRecentNormalModeFile;
    extern int& MostRecentMasterModeFile;

    extern bool& LoadedSavefile;
    extern bool& GameIsRunning;
    extern bool& NoSavefileForUser;
    extern bool& MasterModeFileExists;
    extern bool& MasterModeFileLoaded;
    extern bool& HasDLC;

    extern int& MasterModeSlot;
};
