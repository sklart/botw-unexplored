#include <fstream>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <thread>
#include <chrono>
#include <dirent.h>

#include <switch.h>

#include "SavefileIO.h"
#include "Graphics/InitGL.h"
#include "Graphics/Font.h"
#include "Map.h"
#include "Accounts.h"
#include "Dialog.h"
#include "Legend.h"
#include "Localization.h"
#include "Log.h"
#include "MapObject.hpp"
#include "Settings.h"
#include "ManualProgress.h"

bool openGLInitialized = false;
bool nxLinkInitialized = false;
bool socketInitialized = false;
int s_nxlinkSock = -1;
bool cleanupComplete = false;

static SettingsIO::Settings GetCurrentSettings()
{
    SettingsIO::Settings settings;
    settings.cameraX = Map::m_CameraPosition.x;
    settings.cameraY = Map::m_CameraPosition.y;
    settings.zoom = Map::m_Zoom;
    settings.legendOpen = Map::m_Legend->m_IsOpen;
    for (int i = 0; i < IconButton::ButtonTypes::Count; ++i)
        settings.visible[i] = Map::m_Legend->m_Show[i];
    settings.showMode = static_cast<int>(Map::m_Legend->m_ShowMode);
    settings.language = static_cast<int>(Localization::GetLanguage());
    return settings;
}

static bool SaveSettings()
{
    std::ofstream file("sdmc:/switch/botw-unexplored/settings.txt");
    return file.is_open() && SettingsIO::Save(file, GetCurrentSettings());
}

static bool SaveManualProgress()
{
    std::ofstream file("sdmc:/switch/botw-unexplored/manual_progress.dat");
    return file.is_open() && ManualProgress::Save(file);
}

static void ApplySettings(const SettingsIO::Settings& settings)
{
    Map::m_CameraPosition.x = std::max(-4250.0f, std::min(4250.0f, settings.cameraX));
    Map::m_CameraPosition.y = std::max(-1750.0f, std::min(2250.0f, settings.cameraY));
    Map::m_CursorPosition = Map::m_CameraPosition;
    Map::m_Zoom = std::max(0.1f, std::min(10.0f, settings.zoom));
    Map::m_Legend->m_IsOpen = settings.legendOpen;
    Map::m_Legend->m_ShowMode = static_cast<ShowMode>(settings.showMode);
    for (int i = 0; i < IconButton::ButtonTypes::Count; ++i)
        Map::m_Legend->m_Buttons[i]->Click(Map::m_Legend, settings.visible[i]);
    if (settings.language >= 0 && settings.language <= static_cast<int>(Localization::Language::Spanish))
        Localization::SetLanguage(static_cast<Localization::Language>(settings.language));
}

static void deinitNxLink()
{
    if (s_nxlinkSock >= 0)
    {
        close(s_nxlinkSock);
        s_nxlinkSock = -1;
    }
}

void cleanUp()
{
    if (cleanupComplete)
        return;
    cleanupComplete = true;

    // Save settings
    if (!SavefileIO::DirectoryExists("sdmc:/switch/botw-unexplored"))
        mkdir("sdmc:/switch/botw-unexplored", 0777);

    if (Map::m_Legend != nullptr && SaveSettings())
        Log("Saved settings");
    else
        Log("Failed top open settings file (cleanUp())");

    if (SaveManualProgress())
        Log("Saved manual progress");
    else
        Log("Failed to save manual progress");

    Log("SHUTDOWN: map cleanup begin");
    Map::Destroy();
    Log("SHUTDOWN: map cleanup complete");

    // Cleanup
    Log("SHUTDOWN: romfs cleanup begin");
    if (openGLInitialized)
        romfsExit();
    Log("SHUTDOWN: EGL cleanup begin");

    // Deinitialize EGL
    if (openGLInitialized)
    {
        deinitEgl();
        openGLInitialized = false;
    }
    Log("SHUTDOWN: EGL cleanup complete");

    // Deinitialize network
    deinitNxLink();
    if (socketInitialized)
    {
        socketExit();
        socketInitialized = false;
    }
}

int main()
{
    LogInit();
    Log("START: main");

    // To be able to run memory cleanup when the app closes
    if (R_FAILED(appletLockExit()))
        Log("appletLockExit() failed");
    Log("START: applet lock complete");

    // Setup NXLink
    socketInitializeDefault();
    socketInitialized = true;
    s_nxlinkSock = nxlinkStdio();
    nxLinkInitialized = true;
    Log("START: network complete");

    // Init romfs
    if (R_FAILED(romfsInit()))
        Log("romfsInit() failed");
    Log("START: RomFS complete");

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // OpenGL
    // Initialize EGL on the default window
    openGLInitialized = initEgl(nwindowGetDefault());
    if (!openGLInitialized)
    {
        Log("OpenGL Failed to initialize");
        return 0;
    }
    Log("START: EGL complete");

    // Load OpenGL routines using glad
    gladLoadGL();
    Log("START: GL loader complete");

    // OpenGL config
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    if (!Map::Init())
    {
        Log("START: map initialization failed");
        cleanUp();
        appletUnlockExit();
        return EXIT_FAILURE;
    }
    Map::m_Pad = &pad;

    // Load settings if they exist. Legacy settings are immediately rewritten in the versioned format.
    std::ifstream settingsFile("sdmc:/switch/botw-unexplored/settings.txt");
    if (settingsFile.is_open())
    {
        SettingsIO::Settings settings;
        const SettingsIO::LoadResult result = SettingsIO::Load(settingsFile, settings);
        if (result == SettingsIO::LoadResult::Current || result == SettingsIO::LoadResult::Legacy)
        {
            ApplySettings(settings);
            if (result == SettingsIO::LoadResult::Legacy && !SaveSettings())
                Log("Failed to migrate legacy settings");
        }
        else if (result == SettingsIO::LoadResult::FutureVersion)
            Log("Settings file uses an unsupported future version");
        else if (result == SettingsIO::LoadResult::Invalid)
            Log("Ignoring invalid settings file");
    }
    else
    {
        Log("Failed to open settings file");
    }

    settingsFile.close();

    std::ifstream manualProgressFile("sdmc:/switch/botw-unexplored/manual_progress.dat");
    if (manualProgressFile.is_open() && !ManualProgress::Load(manualProgressFile))
        Log("Ignoring invalid manual progress file");

    bool hasDoneFirstDraw = false;
    bool hasLoadedSave = false;

    while (appletMainLoop())
    {
        // Scan the gamepad. This should be done once for each frame
        padUpdate(&pad);

        u64 buttonsDown = padGetButtonsDown(&pad);

        if (buttonsDown & HidNpadButton_Plus)
            break;

        if (Map::m_ShouldExit)
            break;

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Update
        Map::Update();

        // // Render
        Map::Render();

        if (hasDoneFirstDraw && !hasLoadedSave)
        {
            hasLoadedSave = true;
            Log("LoadGamesave() status:", SavefileIO::LoadGamesave() ? "true" : "false");

            Map::UpdateMapObjects();

        }

        eglSwapBuffers(s_display, s_surface);

#ifdef BOTW_DEBUG_GL
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
            Log("OpenGL error", (int)err);
#endif

        hasDoneFirstDraw = true;
    }

    Log("Exiting...");

    cleanUp();

    appletUnlockExit(); // Exit the app

    return EXIT_SUCCESS;
}
