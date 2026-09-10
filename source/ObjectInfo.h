#pragma once

#include <string>

#include "ObjectTypes.h"
#include "Graphics/Quad.h"

class ObjectInfo
{
public:
    ObjectInfo();
    void SetObject(Data::ObjectType type, uint32_t completionHash, const glm::vec2& mapPosition,
                   const std::string& name, bool* found);
    void SetOpen(bool open);
    void Render(glm::mat4 projection = glm::mat4(1.0f));
    bool IsPositionOn(const glm::vec2& position) const;
    glm::vec2 GetMapPosition() const;
    ~ObjectInfo();

    bool m_IsOpen = false;
    Data::ObjectType m_Type = Data::ObjectType::Korok;
    uint32_t m_CompletionHash = 0;
    bool* m_Found = nullptr;

private:
    Quad m_Background;
    TexturedQuad* m_Icon = nullptr;
    glm::vec2 m_MapPosition = glm::vec2(0.0f);
    std::string m_Name;
};
