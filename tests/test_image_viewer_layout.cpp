#include "ImageViewerLayout.h"

#include <cassert>

int main()
{
    const ImageViewerLayout::Fit landscape = ImageViewerLayout::FitInside(400.0f, 200.0f, 1200.0f, 640.0f);
    assert(landscape.scale == 3.0f);
    assert(landscape.width == 1200.0f);
    assert(landscape.height == 600.0f);

    const ImageViewerLayout::Fit portrait = ImageViewerLayout::FitInside(200.0f, 400.0f, 1200.0f, 640.0f);
    assert(portrait.scale == 1.6f);
    assert(portrait.width == 320.0f);
    assert(portrait.height == 640.0f);

    const ImageViewerLayout::Fit invalid = ImageViewerLayout::FitInside(0.0f, 400.0f, 1200.0f, 640.0f);
    assert(invalid.scale == 0.0f);
    assert(invalid.width == 0.0f);
    assert(invalid.height == 0.0f);
}
