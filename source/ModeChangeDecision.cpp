#include "ModeChangeDecision.h"

bool ModeChangeDecision::Resolve(bool currentMode, bool requestedMode, bool loadSucceeded)
{
    return loadSucceeded ? requestedMode : currentMode;
}
