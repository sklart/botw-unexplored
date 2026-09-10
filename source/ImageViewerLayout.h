#pragma once

namespace ImageViewerLayout
{
    struct Fit
    {
        float scale = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    Fit FitInside(float imageWidth, float imageHeight, float availableWidth, float availableHeight);
}
