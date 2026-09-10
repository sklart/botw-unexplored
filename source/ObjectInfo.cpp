#include "ObjectInfo.h"

#include "Localization.h"
#include "Legend.h"
#include "Map.h"
#include "ObjectModel.h"
#include "ManualProgress.h"
#include "SavefileIO.h"

ObjectInfo::ObjectInfo()
{
    m_Background.Create(glm::vec2(Map::m_ScreenLeft, Map::m_ScreenBottom),
                        glm::vec2(Map::m_ScreenLeft + 390.0f, Map::m_ScreenBottom),
                        glm::vec2(Map::m_ScreenLeft + 390.0f, Map::m_ScreenTop),
                        glm::vec2(Map::m_ScreenLeft, Map::m_ScreenTop));
    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);
}

void ObjectInfo::SetObject(Data::ObjectType type, uint32_t completionHash, const glm::vec2& mapPosition,
                           const std::string& name, bool* found)
{
    m_Type = type;
    m_CompletionHash = completionHash;
    m_MapPosition = mapPosition;
    m_Name = name;
    m_Found = found;

    delete m_Icon;
    m_Icon = nullptr;
    const ObjectModel::TypeMetadata& metadata = ObjectModel::GetMetadata(type);
    if (metadata.iconPath[0] != '\0')
    {
        m_Icon = new TexturedQuad();
        m_Icon->Create(metadata.iconPath);
        m_Icon->m_Scale = type == Data::ObjectType::Location ? 0.035f : 0.8f;
    }
}

void ObjectInfo::SetOpen(bool open)
{
    if (open)
        Map::CloseInfoPanels();
    m_IsOpen = open;
    if (open)
        Map::m_Legend->m_IsOpen = false;
}

void ObjectInfo::Render(glm::mat4 projection)
{
    if (!m_IsOpen)
        return;

    m_Background.m_ProjectionMatrix = &projection;
    m_Background.m_ViewMatrix = nullptr;
    m_Background.Render();

    if (m_Icon && m_Icon->m_Texture)
    {
        m_Icon->m_ProjectionMatrix = &projection;
        m_Icon->m_ViewMatrix = nullptr;
        m_Icon->m_Position = glm::vec2(Map::m_ScreenLeft + 340.0f, Map::m_ScreenTop - 100.0f);
        m_Icon->Render();
    }

    const float left = Map::m_ScreenLeft + 25.0f;
    float y = Map::m_ScreenTop - 60.0f;
    Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::ObjectInfo), glm::vec2(left, y), 0.45f, glm::vec3(1.0f));
    y -= 55.0f;
    Map::m_Font.AddTextToBatch(Localization::GetObjectTypeName(m_Type), glm::vec2(left, y), 0.68f, glm::vec3(1.0f), ALIGN_LEFT, 285.0f);
    y -= 62.0f;
    if (!m_Name.empty())
    {
        const glm::vec2 size = Map::m_Font.AddTextToBatch(m_Name, glm::vec2(left, y), 0.42f, glm::vec3(1.0f), ALIGN_LEFT, 340.0f);
        y -= size.y + 25.0f;
    }
    Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::Position), glm::vec2(left, y), 0.42f, glm::vec3(1.0f));
    y -= 35.0f;
    const int gameX = static_cast<int>(m_MapPosition.x / 0.25f);
    const int gameY = static_cast<int>(-m_MapPosition.y / 0.25f);
    Map::m_Font.AddTextToBatch(std::to_string(gameX) + ", " + std::to_string(gameY), glm::vec2(left, y), 0.42f, glm::vec3(1.0f));
    y -= 55.0f;
    Map::m_Font.AddTextToBatch(m_Found && *m_Found ? Localization::Get(Localization::Text::Found) : Localization::Get(Localization::Text::NotFound),
                              glm::vec2(left, y), 0.46f, glm::vec3(1.0f));
    if (SavefileIO::GameIsRunning && m_Found && !*m_Found && ManualProgress::CanPersist())
        Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::MarkFound), glm::vec2(left, Map::m_ScreenBottom + 45.0f), 0.35f, glm::vec3(1.0f));
    Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::Close), glm::vec2(Map::m_ScreenLeft + 370.0f, Map::m_ScreenBottom + 45.0f), 0.35f, glm::vec3(1.0f), ALIGN_RIGHT);
}

bool ObjectInfo::IsPositionOn(const glm::vec2& position) const
{
    return position.x >= Map::m_ScreenLeft && position.x <= Map::m_ScreenLeft + 390.0f &&
           position.y >= Map::m_ScreenBottom && position.y <= Map::m_ScreenTop;
}

glm::vec2 ObjectInfo::GetMapPosition() const
{
    return m_MapPosition;
}

ObjectInfo::~ObjectInfo()
{
    delete m_Icon;
}
