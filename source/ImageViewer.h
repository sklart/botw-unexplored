#pragma once

#include "Graphics/Quad.h"

class ImageViewer
{
public:
    ImageViewer();

    void Open(TexturedQuad* image);
    void Close();
    bool IsOpen() const;
    void Render(glm::mat4& projection);

private:
    Quad m_Background;
    TexturedQuad* m_Image = nullptr;
    bool m_IsOpen = false;
};
