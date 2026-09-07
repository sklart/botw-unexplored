#include "Navigation.h"

#include <algorithm>
#include <limits>

Navigation::Point Navigation::ClampToViewport(Point cursor, const Viewport& viewport)
{
    cursor.x = std::max(viewport.center.x - viewport.halfWidth, std::min(viewport.center.x + viewport.halfWidth, cursor.x));
    cursor.y = std::max(viewport.center.y - viewport.halfHeight, std::min(viewport.center.y + viewport.halfHeight, cursor.y));
    return cursor;
}

bool Navigation::FindNearest(const std::vector<Candidate>& candidates, Point origin, bool missingOnly, Point& result)
{
    float bestDistanceSquared = std::numeric_limits<float>::max();
    bool foundCandidate = false;
    for (const Candidate& candidate : candidates)
    {
        if (!candidate.visible || (missingOnly && candidate.found))
            continue;
        const float dx = candidate.position.x - origin.x;
        const float dy = candidate.position.y - origin.y;
        const float distanceSquared = dx * dx + dy * dy;
        if (distanceSquared < bestDistanceSquared)
        {
            bestDistanceSquared = distanceSquared;
            result = candidate.position;
            foundCandidate = true;
        }
    }
    return foundCandidate;
}
