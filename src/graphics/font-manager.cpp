#include <ftgl/texture-atlas.h>
#include <ftgl/freetype-gl.h>

#include <oni-core/components/visual.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace graphics {

        FontManager::FontManager(std::string font, int size, float gameWidth, float gameHeight)
//                m_FTAtlas(, ftgl::texture_atlas_delete),
//                m_FTFont(ftgl::texture_font_new_from_file(m_FTAtlas.get(), 10, "resources/fonts/Vera.ttf"),
//                         ftgl::texture_font_delete) {
        {

            m_FTAtlas = ftgl::texture_atlas_new(512, 512, 1);
            m_FTFont = ftgl::texture_font_new_from_file(m_FTAtlas, size, font.c_str());

            mGameWidth = gameWidth;
            mGameHeight = gameHeight;

            std::string cache = " !\"#$%&'()*+,-./0123456789:;<=>?@"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

            auto glyph = ftgl::texture_font_load_glyphs(m_FTFont, cache.c_str());

            ONI_DEBUG_ASSERT(glyph == 0);

            m_FTAtlas->id = graphics::Texture::load(*this);

        }

        FontManager::~FontManager() {
            ftgl::texture_atlas_delete(m_FTAtlas);
            ftgl::texture_font_delete(m_FTFont);
        }

        const ftgl::texture_glyph_t *FontManager::findGlyph(const char &character) const {
            auto glyph = ftgl::texture_font_find_glyph(m_FTFont, &character);

            // Make sure the character is pre-loaded and valid.
            ONI_DEBUG_ASSERT(glyph);
            return glyph;
        }

        size_t FontManager::getAtlasWidth() const {
            return m_FTAtlas->width;
        }

        size_t FontManager::getAtlasHeight() const {
            return m_FTAtlas->height;
        }

        unsigned char *FontManager::getAtlasData() const {
            return m_FTAtlas->data;
        }

        GLuint FontManager::getTextureID() const { return m_FTAtlas->id; }

        components::Text FontManager::createTextFromString(const std::string &text, const math::vec3 &position) {
            auto textComponent = components::Text();
            for (auto character: text) {
                auto glyph = findGlyph(character);
                textComponent.height.emplace_back(glyph->height);
                textComponent.width.emplace_back(glyph->width);
                textComponent.offsetX.emplace_back(glyph->offset_x);
                textComponent.offsetY.emplace_back(glyph->offset_y);
                textComponent.advanceX.emplace_back(glyph->advance_x);
                textComponent.advanceY.emplace_back(glyph->advance_y);
                textComponent.uv.emplace_back(math::vec4{glyph->s0, glyph->t0, glyph->s1, glyph->t1});
                textComponent.xScaling = mGameWidth;
                textComponent.yScaling = mGameHeight;
            }
            textComponent.textureID = m_FTAtlas->id;
            textComponent.textContent = text;
            textComponent.position = position;
            return textComponent;
        }

        void FontManager::updateText(const std::string &textContent, components::Text &text) {
            text = createTextFromString(textContent, text.position);
        }
    }
}