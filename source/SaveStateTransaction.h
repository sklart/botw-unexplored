#pragma once

namespace SaveStateTransaction
{
    template <typename State>
    bool Commit(State& liveState, const State& candidateState, bool candidateIsValid)
    {
        if (!candidateIsValid)
            return false;
        liveState = candidateState;
        return true;
    }
}
