#pragma once

#include <memory>

#include <ftgl/texture-atlas.h>
#include <ftgl/freetype-gl.h>

namespace oni {
    namespace graphics {
        class FontManager {

            // Wrap atlas and font with unique_ptr and pass the custom deleter
//            std::unique_ptr<ftgl::texture_font_t, decltype(&ftgl::texture_font_delete)> m_FTFont;
//            std::unique_ptr<ftgl::texture_atlas_t, decltype(&ftgl::texture_atlas_delete)> m_FTAtlas;
            ftgl::texture_atlas_t *m_FTAtlas;
            ftgl::texture_font_t *m_FTFont;

        public:
            explicit FontManager(std::string font);

            ~FontManager() {
                ftgl::texture_atlas_delete(m_FTAtlas);
                ftgl::texture_font_delete(m_FTFont);
            }

            ftgl::texture_atlas_t *getAtlas() {
                return m_FTAtlas;
            };

            ftgl::texture_font_t *getFont() const {
                return m_FTFont;
            };

            void setAtlasID(unsigned int id) {
                m_FTAtlas->id = id;
            }
        };
    }
}