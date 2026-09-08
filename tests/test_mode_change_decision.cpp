#include <cassert>

#include "ModeChangeDecision.h"

int main()
{
    // A selected profile with a successful Normal Mode load commits the requested mode.
    assert(ModeChangeDecision::Resolve(true, false, true) == false);

    // An unfinished profile selection leaves the existing Master Mode state intact.
    assert(ModeChangeDecision::Resolve(true, false, false) == true);

    // A failed load keeps Normal Mode unchanged.
    assert(ModeChangeDecision::Resolve(false, false, false) == false);

    // Master-to-Normal and Normal-to-Master changes commit after successful loads.
    assert(ModeChangeDecision::Resolve(true, false, true) == false);
    assert(ModeChangeDecision::Resolve(false, true, true) == true);

    // A failed Master Mode load leaves Normal Mode selected.
    assert(!ModeChangeDecision::Resolve(false, true, false));
}
