#include "Font.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glad/glad.h>

#include "../Log.h"
#include "BasicVertices.h"

std::u32string DecodeUtf8(const std::string& text)
{
    std::u32string result;
    for (size_t i = 0; i < text.size();)
    {
        const unsigned char first = static_cast<unsigned char>(text[i]);
        uint32_t codepoint = first;
        size_t length = 1;
        if ((first & 0xE0) == 0xC0 && i + 1 < text.size()) { codepoint = ((first & 0x1F) << 6) | (static_cast<unsigned char>(text[i + 1]) & 0x3F); length = 2; }
        else if ((first & 0xF0) == 0xE0 && i + 2 < text.size()) { codepoint = ((first & 0x0F) << 12) | ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 6) | (static_cast<unsigned char>(text[i + 2]) & 0x3F); length = 3; }
        else if ((first & 0xF8) == 0xF0 && i + 3 < text.size()) { codepoint = ((first & 0x07) << 18) | ((static_cast<unsigned char>(text[i + 1]) & 0x3F) << 12) | ((static_cast<unsigned char>(text[i + 2]) & 0x3F) << 6) | (static_cast<unsigned char>(text[i + 3]) & 0x3F); length = 4; }
        result.push_back(codepoint);
        i += length;
    }
    return result;
}

size_t split(const std::u32string& txt, std::vector<std::u32string>& strs, char32_t ch)
{
    size_t pos = txt.find(ch);
    size_t initialPos = 0;
    strs.clear();
    while (pos != std::u32string::npos) { strs.push_back(txt.substr(initialPos, pos - initialPos)); initialPos = pos + 1; pos = txt.find(ch, initialPos); }
    strs.push_back(txt.substr(initialPos));
    return strs.size();
}

int Font::Load(const std::string& filepath)
{
    Log("FONT: begin");
    if (FT_Init_FreeType(&m_FreeType))
    {
        Log("Freetype: Could not init FreeType Library");
        return -1;
    }
    Log("FONT: FreeType initialized");

    if (FT_New_Face(m_FreeType, filepath.c_str(), 0, &m_Face))
    {
        Log("Freetype: Failed to load font");
        FT_Done_FreeType(m_FreeType);
        m_FreeType = nullptr;
        return -1;
    }
    Log("FONT: face loaded");

    FT_Set_Pixel_Sizes(m_Face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::string vertexShaderSource = R"text(
        #version 330 core
        layout (location = 0) in vec3 vertex;
        layout (location = 2) in vec2 textureCoords;
        out vec2 TexCoords;
        uniform mat4 u_ProjectionMatrix = mat4(1.0);
        uniform mat4 u_ViewMatrix = mat4(1.0);
        uniform mat4 u_ModelMatrix = mat4(1.0);
        uniform vec2 u_Size = vec2(1.0, 1.0);
        void main()
        {
            gl_Position = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(vertex.xy * u_Size, 0.0, 1.0);
            TexCoords = textureCoords;
        }
    )text";

    std::string fragmentShaderSource = R"text(
        #version 330 core
        in vec2 TexCoords;
        out vec4 color;
        uniform sampler2D text;
        uniform vec3 textColor = vec3(1.0, 1.0, 1.0);
        void main()
        {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
            color = vec4(textColor, 1.0) * sampled;
        }
    )text";

    m_Shader = ShaderLoader::CreateShaderFromSource(vertexShaderSource, fragmentShaderSource);
    if (m_Shader.m_id == 0)
    {
        Log("FONT: shader creation failed");
        return -1;
    }

    glm::vec3 vertexPositions[4] = {
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f)
    };
    uint16_t indices[6] = {0, 1, 2, 0, 2, 3};
    glm::vec2 textureCoords[4] = {
        glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
    };
    for (int i = 0; i < 4; i++)
    {
        TextureVertex vertex;
        vertex.position = vertexPositions[i];
        vertex.textureCoord = textureCoords[i];
        m_Mesh.AddVertex(vertex);
    }
    for (int i = 0; i < 6; i++)
        m_Mesh.AddIndex(indices[i]);
    m_Mesh.Update();

    const int maxCountOfEachChar = 300;
    m_CharMesh.m_UseDynamicBuffer = true;
    m_CharMesh.m_DynamicBufferSize = sizeof(TextureVertex) * 4 * maxCountOfEachChar;
    m_CharMesh.CreateEmptyBuffer();

    m_Initialized = true;
    if (!LoadGlyph(' '))
    {
        Log("FONT: failed to load fallback glyph");
        m_Initialized = false;
        return -1;
    }

    Log("Loaded font", filepath.c_str());
    return 1;
}

bool Font::LoadGlyph(uint32_t character)
{
    if (m_Characters.find(character) != m_Characters.end())
        return true;

    if (m_Face == nullptr || FT_Load_Char(m_Face, character, FT_LOAD_RENDER))
    {
        Log("Freetype: Failed to load glyph", static_cast<int>(character));
        return false;
    }

    unsigned int texture = 0;
    glActiveTexture(GL_TEXTURE0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows,
                 0, GL_RED, GL_UNSIGNED_BYTE, m_Face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    m_Characters.emplace(character, Character{
        texture,
        glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
        glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
        static_cast<unsigned int>(m_Face->glyph->advance.x)
    });
    return true;
}

const Character& Font::GetCharacter(uint32_t character)
{
    if (LoadGlyph(character))
        return m_Characters.at(character);
    if (character != '?')
        return GetCharacter('?');

    static const Character empty = {0, glm::ivec2(0), glm::ivec2(0), 0};
    return empty;
}

glm::vec2 Font::RenderText(const std::string& text, glm::vec2 position, float scale, glm::vec3 color, int align)
{
    m_Shader.Bind();
    m_Shader.SetUniform("text", 0);
    m_Shader.SetUniform("textColor", color);
    m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);
    glActiveTexture(GL_TEXTURE0);

    const glm::vec2 startPos(position.x, position.y);
    glm::vec2 textSize(0.0f);
    const std::u32string unicodeText = DecodeUtf8(text);
    std::vector<glm::vec2> characterPositions(unicodeText.length());
    for (size_t i = 0; i < unicodeText.length(); i++)
    {
        const Character& ch = GetCharacter(unicodeText[i]);
        characterPositions[i] = glm::vec2(position.x + ch.Bearing.x * scale, position.y - (ch.Size.y - ch.Bearing.y) * scale);
        position.x += (ch.Advance >> 6) * scale;
    }
    textSize.x = position.x - startPos.x;

    for (size_t i = 0; i < unicodeText.length(); i++)
    {
        const Character& ch = GetCharacter(unicodeText[i]);
        glm::vec2 pos = characterPositions[i];
        if (align == ALIGN_CENTER) pos.x -= textSize.x / 2;
        else if (align == ALIGN_RIGHT) pos.x -= textSize.x;

        const float w = ch.Size.x * scale;
        const float h = ch.Size.y * scale;
        if (h > textSize.y) textSize.y = h;
        glm::mat4 modelMatrix(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(pos, 0.0f));
        m_Shader.SetUniform("u_ModelMatrix", modelMatrix);
        m_Shader.SetUniform("u_Size", glm::vec2(w, h));
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        m_Mesh.Render();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    m_Shader.Unbind();
    return textSize;
}

void Font::BeginBatch()
{
    m_CharsToRender.clear();
}

glm::vec2 Font::AddTextToBatch(const std::string& text, glm::vec2 position, float scale, glm::vec3 color, int align, float maxWidth)
{
    glm::vec2 itPosition(position);
    const glm::vec2 startPosition(position);
    glm::vec2 textSize(0.0f);
    const std::u32string unicodeText = DecodeUtf8(text);
    for (uint32_t character : unicodeText)
        GetCharacter(character);

    std::vector<std::u32string> words;
    split(unicodeText, words, U' ');
    std::vector<glm::vec2> charPositions;
    for (std::u32string& word : words)
    {
        glm::vec2 itPositionWord = itPosition;
        for (size_t c = 0; c < word.length() + 1; c++)
            itPositionWord.x += (GetCharacter(c == word.length() ? ' ' : word[c]).Advance >> 6) * scale;

        float wordRightX = itPositionWord.x;
        if (!word.empty())
            wordRightX += GetCharacter(word.back()).Bearing.x * scale;

        for (size_t c = 0; c < word.length() + 1; c++)
        {
            const Character& ch = GetCharacter(c == word.length() ? ' ' : word[c]);
            if (maxWidth > 0.0f && wordRightX > startPosition.x + maxWidth && itPositionWord.y == itPosition.y)
            {
                itPosition.x = startPosition.x;
                itPosition.y -= 50.0f * scale;
            }
            charPositions.emplace_back(itPosition.x + ch.Bearing.x * scale, itPosition.y - (ch.Size.y - ch.Bearing.y) * scale);
            if (ch.Size.y * scale > textSize.y) textSize.y = ch.Size.y * scale;
            itPosition.x += (ch.Advance >> 6) * scale;
        }
    }
    textSize = glm::abs(itPosition - startPosition);

    for (size_t i = 0; i < unicodeText.length(); i++)
    {
        glm::vec2 charPosition = charPositions[i];
        if (align == ALIGN_CENTER) charPosition.x -= textSize.x / 2;
        else if (align == ALIGN_RIGHT) charPosition.x -= textSize.x;
        m_CharsToRender[unicodeText[i]].emplace_back(unicodeText[i], charPosition, scale, color, align);
    }
    return textSize;
}

void Font::RenderBatch()
{
    glm::vec2 textureCoords[4] = {
        glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f),
        glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)
    };
    m_Shader.Bind();
    m_Shader.SetUniform("text", 0);
    m_Shader.SetUniform("u_ProjectionMatrix", *m_ProjectionMatrix);
    m_Shader.SetUniform("u_ViewMatrix", *m_ViewMatrix);

    for (auto& entry : m_CharsToRender)
    {
        std::vector<Text>& texts = entry.second;
        const Character& ch = GetCharacter(entry.first);
        m_CharMesh.Clear();
        for (const Text& text : texts)
        {
            const float w = ch.Size.x * text.scale;
            const float h = ch.Size.y * text.scale;
            glm::vec3 vertexPositions[4];
            BasicVertices::Quad::ConstructFromBottomleft(vertexPositions, text.position, w, h);
            for (int i = 0; i < 4; i++)
            {
                TextureVertex vertex;
                vertex.position = vertexPositions[i];
                vertex.textureCoord = textureCoords[i];
                m_CharMesh.AddVertex(vertex);
            }
            for (int i = 0; i < 6; i++)
                m_CharMesh.AddIndex(BasicVertices::Quad::Indices[i] + m_CharMesh.GetVertices().size() - 4);
        }
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        m_Shader.SetUniform("u_ModelMatrix", glm::mat4(1.0f));
        m_Shader.SetUniform("u_Size", glm::vec2(1.0f));
        m_Shader.SetUniform("textColor", texts[0].color);
        m_CharMesh.Update();
        m_CharMesh.Render();
    }
    m_CharsToRender.clear();
    m_Shader.Unbind();
}

void Font::Destroy()
{
    m_Shader.Delete();
    for (const auto& entry : m_Characters)
        glDeleteTextures(1, &entry.second.TextureID);
    m_Characters.clear();
    m_Mesh.Destroy();
    m_CharMesh.Destroy();
    if (m_Face != nullptr)
    {
        FT_Done_Face(m_Face);
        m_Face = nullptr;
    }
    if (m_FreeType != nullptr)
    {
        FT_Done_FreeType(m_FreeType);
        m_FreeType = nullptr;
    }
    m_Initialized = false;
}

Font::~Font()
{
    Destroy();
}
