#pragma once

#include <vector>

namespace Navigation
{
    struct Point
    {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Viewport
    {
        Point center;
        float halfWidth = 0.0f;
        float halfHeight = 0.0f;
    };

    struct Candidate
    {
        Point position;
        bool visible = true;
        bool found = false;
    };

    Point ClampToViewport(Point cursor, const Viewport& viewport);
    bool FindNearest(const std::vector<Candidate>& candidates, Point origin, bool missingOnly, Point& result);
}
