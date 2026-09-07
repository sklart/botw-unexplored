#include "MapLocation.h"

#include <algorithm>
#include <switch.h>

#include "Map.h"
#include "Legend.h"
#include "Localization.h"
#include "Graphics/BasicVertices.h"
#include "SavefileIO.h"

MapLocation::MapLocation()
{

}

void MapLocation::Init()
{
    // // Set the fonts matrices
    // m_Font->m_ProjectionMatrix = &m_Map->m_ProjectionMatrix;
    // m_Font->m_ViewMatrix = &m_Map->m_ViewMatrix;

    // // Set text font
    // m_Text.m_Font = m_Font;

    // Create text mesh
    //m_Text.Create(m_LocationData->displayName);
}

void MapLocation::Update()
{
    m_Scale = 0.25f / Map::m_Zoom;

    float minScale = 0.25f;
    if (m_Scale < minScale)
        m_Scale = minScale;
}

void MapLocation::Render()
{
    if (!Map::m_Legend->ShouldShow(m_Found)) return;

    // The overview is too dense for readable labels. They become available while zooming in.
    if (Map::m_Zoom < 0.55f) return;

    const std::string label = Localization::GetLocationName(m_LocationData->hash, m_LocationData->displayName);
    const size_t characterCount = std::max<size_t>(1, std::count_if(label.begin(), label.end(), [](unsigned char c)
    {
        return (c & 0xC0) != 0x80;
    }));

    // Keep long translated names within a predictable on-screen width.
    constexpr float MaxLabelWidthOnScreen = 190.0f;
    constexpr float EstimatedGlyphWidth = 30.0f;
    float textScale = m_Scale;
    const float estimatedWidth = characterCount * EstimatedGlyphWidth * textScale * Map::m_Zoom;
    if (estimatedWidth > MaxLabelWidthOnScreen)
        textScale *= MaxLabelWidthOnScreen / estimatedWidth;

    const float halfWidth = std::min(MaxLabelWidthOnScreen * 0.5f, characterCount * EstimatedGlyphWidth * textScale * Map::m_Zoom * 0.5f);
    constexpr float HalfHeight = 18.0f;
    const glm::vec2 screenPosition = (m_Position - Map::m_CameraPosition) * Map::m_Zoom;
    const glm::vec4 bounds(screenPosition.x - halfWidth, screenPosition.y - HalfHeight,
                           screenPosition.x + halfWidth, screenPosition.y + HalfHeight);

    if (!Map::IsInView(m_Position, halfWidth / Map::m_Zoom)) return;

    for (const glm::vec4& other : m_LabelBounds)
    {
        const bool overlaps = bounds.x < other.z && bounds.z > other.x &&
                              bounds.y < other.w && bounds.w > other.y;
        if (overlaps) return;
    }
    m_LabelBounds.push_back(bounds);

    Map::m_Font.AddTextToBatch(label, m_Position, textScale, m_Color, ALIGN_CENTER);
}

void MapLocation::BeginLabelPass()
{
    m_LabelBounds.clear();
}

MapLocation::~MapLocation()
{
    
}

bool MapLocation::m_ShowAnyway = false;
std::vector<glm::vec4> MapLocation::m_LabelBounds;
