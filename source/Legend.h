#pragma once

#include "Graphics/Quad.h"
#include "LegendCategories.h"

class Legend;

enum class ShowMode
{
    Missing = 0,
    Completed = 1,
    All = 2
};

class IconButton
{
public:
    using ButtonTypes = LegendCategories::ButtonTypes;
    static constexpr ButtonTypes Koroks = ButtonTypes::Koroks;
    static constexpr ButtonTypes Shrines = ButtonTypes::Shrines;
    static constexpr ButtonTypes Hinoxes = ButtonTypes::Hinoxes;
    static constexpr ButtonTypes Taluses = ButtonTypes::Taluses;
    static constexpr ButtonTypes Moldugas = ButtonTypes::Moldugas;
    static constexpr ButtonTypes Locations = ButtonTypes::Locations;
    static constexpr ButtonTypes ShowCompleted = ButtonTypes::ShowCompleted;

public:
    IconButton();

    // Position is top-left corner
    IconButton(ButtonTypes type, glm::vec2 position, float width, float height, float iconScale = 1.0f);

    void Render();

    bool Click(Legend* legend);
    bool Click(Legend* legend, bool toggled);

    ~IconButton();

public:
    Quad m_Button;
    TexturedQuad m_Border;
    TexturedQuad m_Icon;
    std::string m_Text;

    bool m_IsSelected = false;
    bool m_IsToggled = false;

    ButtonTypes m_Type = ButtonTypes::Koroks;

    static constexpr glm::vec4 HighlightedColor = glm::vec4(64.0f, 113.0f, 145.0f, 0.8f);
    static constexpr glm::vec4 DefaultColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.9f);
    static constexpr glm::vec4 SelectedColor = glm::vec4(85.0f, 158.0f, 100.0f, 0.6f);

    glm::vec2 m_Position;
    float m_Width = 0.0f;
    float m_Height = 0.0f;

    glm::vec4 m_BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
};

class Legend
{
public:
    Legend();

    void Update();
    void Render();

    bool IsPositionOnLegend(glm::vec2 position);
    void UpdateSelectedButton();
    bool ShouldShow(bool found) const;

    ~Legend();

public:
    Quad m_Background;

    std::vector<IconButton*> m_Buttons;

    float m_Width = 350.0f;

    int m_PrevTouchCount = 0;

    int m_HighlightedButton = 0;

    ShowMode m_ShowMode = ShowMode::Missing;
    bool m_IsOpen = true;

    bool m_Show[LegendCategories::ButtonCount];
};
