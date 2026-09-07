#include "KorokDialog.h"

#include "Data.h"
#include "Map.h"
#include "SavefileIO.h"
#include "Log.h"
#include "Localization.h"

KorokDialog::KorokDialog()
{
    m_Background.Create(
        glm::vec2(Map::m_ScreenLeft, Map::m_ScreenBottom),
        glm::vec2(Map::m_ScreenLeft + Width, Map::m_ScreenBottom),
        glm::vec2(Map::m_ScreenLeft + Width, Map::m_ScreenTop),
        glm::vec2(Map::m_ScreenLeft, Map::m_ScreenTop)
    );
    m_Background.m_Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.6f);
}

void KorokDialog::Render(glm::mat4 projMat, glm::mat4 viewMat)
{
    if (!m_IsOpen)
        return;

    m_Background.m_ProjectionMatrix = &projMat;
    m_Background.m_ViewMatrix = nullptr;

    m_Background.Render();

    // Image
    if (m_Image && m_Image->m_Texture != nullptr)
    {
        m_Image->m_ProjectionMatrix = &projMat;
        m_Image->m_ViewMatrix = nullptr;
        m_Image->Render();
    }

    // Place the text just below the image
    float textY = 0.0f;
    if (m_Image)
        textY = m_Image->m_Position.y - (m_Image->m_Texture->m_Height * m_Image->m_Scale / 2.0f) - 50.0f;
    else
        textY = Map::m_ScreenTop - 110.0f;

    // Render text
    glm::vec2 startPos(Map::m_ScreenLeft + m_Margin, textY);

    Map::m_Font.AddTextToBatch(m_Text, startPos, 0.46f, glm::vec3(1.0f), ALIGN_LEFT, Width - m_Margin * 2);

    // Russian action labels are rendered on separate lines to prevent overlap.
    Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::MarkFound), glm::vec2(Map::m_ScreenLeft + 15, Map::m_ScreenTop - 32.0f), 0.34f, glm::vec3(1.0f), ALIGN_LEFT);
    Map::m_Font.AddTextToBatch(Localization::Get(Localization::Text::Close), glm::vec2(Map::m_ScreenLeft + Width - 20.0f, Map::m_ScreenTop - 62.0f), 0.34f, glm::vec3(1.0f), ALIGN_RIGHT);
}

void KorokDialog::SetOpen(bool open)
{
    m_IsOpen = open;
}

void KorokDialog::SetSeed(int seed, int korokIndex)
{
    m_Text = Localization::GetKorokGuide(seed, Data::KorokInfos.at(seed).text);
    m_KorokIndex = korokIndex;

    // Set image
    std::string seedStr = std::to_string(seed);
    if (seedStr.length() == 1)
        seedStr = "00" + seedStr;
    if (seedStr.length() == 2)
        seedStr = "0" + seedStr;

    // Create a new image, so delete the old one first
    if (m_Image)
        delete m_Image;

    m_Image = nullptr;

    // Don't show images if they don't exist
    // Check the romfs:/ (for release builds) and the sdmc:/ (for development builds)
    std::string path = "";
    if (SavefileIO::DirectoryExists("romfs:/guide") && SavefileIO::FileExists("romfs:/guide/Korok" + seedStr + ".jpg"))
        path = "romfs:/guide/Korok" + seedStr + ".jpg";
    else if (SavefileIO::DirectoryExists("sdmc:/switch/botw-unexplored/guide") && SavefileIO::FileExists("sdmc:/switch/botw-unexplored/guide/Korok" + seedStr + ".jpg"))
        path = "sdmc:/switch/botw-unexplored/guide/Korok" + seedStr + ".jpg";

    // Neither exists
    if (path == "")
        return;

    m_Image = new TexturedQuad();
    m_Image->Create(path);
    m_Image->m_Scale = 1.25f;

    float w = m_Image->m_Texture->m_Width * m_Image->m_Scale;
    float h = m_Image->m_Texture->m_Height * m_Image->m_Scale;

    m_Image->m_Position = glm::vec2(Map::m_ScreenLeft + m_Margin + w / 2.0f, Map::m_ScreenTop - 60.0f - h / 2.0f);
}

void KorokDialog::SetPosition(glm::vec2 position)
{
    m_Background.m_Position = position;
}

glm::vec2 KorokDialog::GetPosition()
{
    return m_Background.m_Position;
}

KorokDialog::~KorokDialog()
{
    delete m_Image;
    m_Image = nullptr;
}
