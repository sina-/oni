#include <oni-core/graphic/oni-graphic-font-manager.h>

#include <cassert>

#include <ftgl/texture-atlas.h>
#include <ftgl/freetype-gl.h>

#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/component/oni-component-geometry.h>


namespace oni {
    FontManager::FontManager(const std::string &font,
                             u8 size,
                             r32 gameWidth,
                             r32 gameHeight) {
        // NOTE: 4 means RGBA which also corresponds to OpenGL texture format GL_RGBA
        // TODO: freetype can't handle depth 4 and it is buggy so I have to stick to 3
        mAtlas = ftgl::texture_atlas_new(512, 512, 3);
        mFont = ftgl::texture_font_new_from_file(mAtlas, size, font.c_str());

        mGameWidth = gameWidth;
        mGameHeight = gameHeight;

        std::string cache = " !\"#$%&'()*+,-./0123456789:;<=>?@"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

        auto glyph = ftgl::texture_font_load_glyphs(mFont, cache.c_str());

        assert(glyph == 0);

        mAtlas->id = TextureManager::load(*this);
    }

    FontManager::~FontManager() {
        ftgl::texture_atlas_delete(mAtlas);
        ftgl::texture_font_delete(mFont);
    }

    const ftgl::texture_glyph_t *
    FontManager::findGlyph(const char &character) const {
        auto glyph = ftgl::texture_font_find_glyph(mFont, &character);

        // Make sure the character is pre-loaded and valid.
        assert(glyph);
        return glyph;
    }

    size_t
    FontManager::getAtlasWidth() const {
        return mAtlas->width;
    }

    size_t
    FontManager::getAtlasHeight() const {
        return mAtlas->height;
    }

    unsigned char *
    FontManager::getAtlasData() const {
        return mAtlas->data;
    }

    oniGLenum
    FontManager::getAtlasColorFormatInternal() const {
        return GL_RGB;
    }

    oniGLenum
    FontManager::getAtlasColorFormat() const {
        return GL_BGR;
    }

    oniGLenum
    FontManager::getAtlasColorType() const {
        return GL_UNSIGNED_BYTE;
    }

    GLuint
    FontManager::getTextureID() const { return mAtlas->id; }

    void
    FontManager::initializeText(Material_Text &text) {
        text.textureID = mAtlas->id;
        assignGlyphs(text);
    }

    void
    FontManager::updateText(std::string &&textContent,
                            Material_Text &text) {
        if (textContent == text.textContent) {
            return;
        }
        text.textContent = std::move(textContent);
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
    FontManager::assignGlyphs(Material_Text &text) {
        for (size i = 0; i < text.textContent.size(); ++i) {
            auto glyph = findGlyph(text.textContent[i]);
            if (glyph) {
                auto kerning = 0.f;
                if (i) {
                    kerning = ftgl::texture_glyph_get_kerning(glyph, &text.textContent[i - 1]);
                }
                text.height.emplace_back(glyph->height);
                text.width.emplace_back(glyph->width);
                text.offsetX.emplace_back(glyph->offset_x + kerning);
                text.offsetY.emplace_back(glyph->offset_y);
                text.advanceX.emplace_back(glyph->advance_x);
                text.advanceY.emplace_back(glyph->advance_y);
                text.uv.emplace_back(vec4{glyph->s0, glyph->t0, glyph->s1, glyph->t1});
                text.xGameScaleDenom = mGameWidth / text.fontSize;
                text.yGameScaleDenom = mGameHeight / text.fontSize;
            }
        }
    }
}