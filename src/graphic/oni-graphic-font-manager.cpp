#include <oni-core/graphic/oni-graphic-font-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <ftgl/freetype-gl.h>

#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/component/oni-component-tag.h>
#include <oni-core/entities/oni-entities-factory.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace graphic {

        FontManager::FontManager(std::string font,
                                 unsigned char size,
                                 common::r32 gameWidth,
                                 common::r32 gameHeight)
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

            assert(glyph == 0);

            m_FTAtlas->id = TextureManager::load(*this);
        }

        FontManager::~FontManager() {
            ftgl::texture_atlas_delete(m_FTAtlas);
            ftgl::texture_font_delete(m_FTFont);
        }

        const ftgl::texture_glyph_t *
        FontManager::findGlyph(const char &character) const {
            auto glyph = ftgl::texture_font_find_glyph(m_FTFont, &character);

            // Make sure the character is pre-loaded and valid.
            assert(glyph);
            return glyph;
        }

        size_t
        FontManager::getAtlasWidth() const {
            return m_FTAtlas->width;
        }

        size_t
        FontManager::getAtlasHeight() const {
            return m_FTAtlas->height;
        }

        unsigned char *
        FontManager::getAtlasData() const {
            return m_FTAtlas->data;
        }

        GLuint
        FontManager::getTextureID() const { return m_FTAtlas->id; }

        common::EntityID
        FontManager::createTextFromString(entities::EntityFactory &entityFactory,
                                          const std::string &text,
                                          const component::WorldP3D &position) {
            auto &entityRegistry = entityFactory.getEntityManager();
            auto entityID = entityFactory.createEntity<entities::EntityType::TEXT>(
                    entities::SimMode::CLIENT,
                    position, text);
            auto &textComponent = entityRegistry.get<component::Text>(entityID);
            assignGlyphs(textComponent);

            return entityID;
        }

        void
        FontManager::updateText(const std::string &textContent,
                                component::Text &text) {
            // TODO: This could be optimized to only update part of text that is actually different.
            text.textContent = textContent;
            text.height.clear();
            text.width.clear();
            text.offsetX.clear();
            text.offsetY.clear();
            text.advanceX.clear();
            text.advanceY.clear();
            text.uv.clear();
            assignGlyphs(text);
        }

        void
        FontManager::assignGlyphs(component::Text &text) {
            for (auto &&character: text.textContent) {
                auto glyph = findGlyph(character);
                text.height.emplace_back(glyph->height);
                text.width.emplace_back(glyph->width);
                text.offsetX.emplace_back(glyph->offset_x);
                text.offsetY.emplace_back(glyph->offset_y);
                text.advanceX.emplace_back(glyph->advance_x);
                text.advanceY.emplace_back(glyph->advance_y);
                text.uv.emplace_back(math::vec4{glyph->s0, glyph->t0, glyph->s1, glyph->t1});
                text.xScaling = mGameWidth;
                text.yScaling = mGameHeight;
            }
            text.textureID = m_FTAtlas->id;
        }
    }
}