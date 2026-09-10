#include "ImageViewerLayout.h"

#include <algorithm>

namespace ImageViewerLayout
{
    Fit FitInside(float imageWidth, float imageHeight, float availableWidth, float availableHeight)
    {
        if (imageWidth <= 0.0f || imageHeight <= 0.0f || availableWidth <= 0.0f || availableHeight <= 0.0f)
            return Fit();

        const float scale = std::min(availableWidth / imageWidth, availableHeight / imageHeight);
        Fit result;
        result.scale = scale;
        result.width = imageWidth * scale;
        result.height = imageHeight * scale;
        return result;
    }
}
