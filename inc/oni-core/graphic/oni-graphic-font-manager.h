#pragma once

#include <string>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/entities/oni-entities-fwd.h>

namespace ftgl {
    class texture_atlas_t;

    class texture_font_t;

    class texture_glyph_t;
}

namespace oni {
    class FontManager {
    public:
        FontManager(const std::string &font,
                    u8 size,
                    r32 gameWidth,
                    r32 gameHeight);

        ~FontManager();

        FontManager(const FontManager &) = delete;

        FontManager &
        operator=(FontManager &) = delete;

        void
        initializeText(Material_Text &);

        void
        updateText(std::string &&textContent,
                   Material_Text &text);

        size_t
        getAtlasWidth() const;

        size_t
        getAtlasHeight() const;

        unsigned char *
        getAtlasData() const;

        oniGLenum
        getAtlasColorFormatInternal() const;

        oniGLenum
        getAtlasColorFormat() const;

        oniGLenum
        getAtlasColorType() const;

        oniGLuint
        getTextureID() const;

    private:
        const ftgl::texture_glyph_t *
        findGlyph(const char &character) const;

        void
        assignGlyphs(Material_Text &);

    private:
        // Wrap atlas and font with unique_ptr and pass the custom deleter
//            std::unique_ptr<ftgl::texture_font_t, decltype(&ftgl::texture_font_delete)> m_FTFont;
//            std::unique_ptr<ftgl::texture_atlas_t, decltype(&ftgl::texture_atlas_delete)> m_FTAtlas;
        ftgl::texture_atlas_t *mAtlas;
        ftgl::texture_font_t *mFont;

        r32 mGameWidth;
        r32 mGameHeight;

    };
}