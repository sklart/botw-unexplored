#include "ImageViewer.h"

#include "Graphics/Texture2D.h"
#include "ImageViewerLayout.h"
#include "Map.h"

ImageViewer::ImageViewer()
{
    m_Background.Create(glm::vec2(Map::m_ScreenLeft, Map::m_ScreenBottom),
                        glm::vec2(Map::m_ScreenRight, Map::m_ScreenBottom),
                        glm::vec2(Map::m_ScreenRight, Map::m_ScreenTop),
                        glm::vec2(Map::m_ScreenLeft, Map::m_ScreenTop));
    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.78f);
}

void ImageViewer::Open(TexturedQuad* image)
{
    if (!image || !image->m_Texture || image->m_Texture->m_Width <= 0 || image->m_Texture->m_Height <= 0)
        return;

    m_Image = image;
    m_IsOpen = true;
}

void ImageViewer::Close()
{
    m_IsOpen = false;
    m_Image = nullptr;
}

bool ImageViewer::IsOpen() const
{
    return m_IsOpen;
}

void ImageViewer::Render(glm::mat4& projection)
{
    if (!m_IsOpen || !m_Image || !m_Image->m_Texture)
        return;

    m_Background.m_ProjectionMatrix = &projection;
    m_Background.m_ViewMatrix = nullptr;
    m_Background.Render();

    const ImageViewerLayout::Fit fit = ImageViewerLayout::FitInside(
        static_cast<float>(m_Image->m_Texture->m_Width),
        static_cast<float>(m_Image->m_Texture->m_Height),
        Map::m_CameraWidth - 80.0f,
        Map::m_CameraHeight - 80.0f);
    if (fit.scale <= 0.0f)
        return;

    glm::mat4* previousProjection = m_Image->m_ProjectionMatrix;
    glm::mat4* previousView = m_Image->m_ViewMatrix;
    const glm::vec2 previousPosition = m_Image->m_Position;
    const float previousScale = m_Image->m_Scale;

    m_Image->m_ProjectionMatrix = &projection;
    m_Image->m_ViewMatrix = nullptr;
    m_Image->m_Position = glm::vec2(0.0f);
    m_Image->m_Scale = fit.scale;
    m_Image->Render();

    m_Image->m_ProjectionMatrix = previousProjection;
    m_Image->m_ViewMatrix = previousView;
    m_Image->m_Position = previousPosition;
    m_Image->m_Scale = previousScale;
}
