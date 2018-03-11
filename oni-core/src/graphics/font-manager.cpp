#include <graphics/font-manager.h>
#include <utils/oni-assert.h>

namespace oni {
    namespace graphics {

        FontManager::FontManager(std::string font)
//                m_FTAtlas(, ftgl::texture_atlas_delete),
//                m_FTFont(ftgl::texture_font_new_from_file(m_FTAtlas.get(), 10, "resources/fonts/Vera.ttf"),
//                         ftgl::texture_font_delete) {
        {

            m_FTAtlas = ftgl::texture_atlas_new(512, 512, 1);
            m_FTFont = ftgl::texture_font_new_from_file(m_FTAtlas, 20, font.c_str());

            std::string cache = " !\"#$%&'()*+,-./0123456789:;<=>?@"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

            auto glyph = ftgl::texture_font_load_glyphs(m_FTFont, cache.c_str());

            ONI_DEBUG_ASSERT(glyph == 0);

        }
    }
}