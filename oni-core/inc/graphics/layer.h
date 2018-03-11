#pragma once

#include <ftgl/texture-atlas.h>
#include <ftgl/texture-font.h>

#include <graphics/renderer-2d.h>
#include <graphics/batch-renderer-2d.h>
#include <graphics/shader.h>
#include <math/mat4.h>
#include <components/visual.h>
#include <entities/world.h>
#include "font-manager.h"

namespace oni {
    namespace graphics {

        static const unsigned long m_MaxNumTextureSamplers = 32;

        /**
         * A Layer is the composition that renders entities in the world. Each entity
         * is assigned a layer. Every rendering property that affects group of entities is defined in
         * this class, e.g., shader, to avoid duplication and unnecessary context switches.
         */
        class Layer {
            // TODO: Using BatchRenderer2D instead of Renderer2D for x0.7 speed up by avoiding
            // polymorphism. Do I really need Renderer2D interface?
            std::unique_ptr<BatchRenderer2D> m_Renderer2D;
            std::unique_ptr<Shader> m_Shader;
            math::mat4 m_ProjectionMatrix;
            std::unique_ptr<graphics::FontManager> m_FontManager;

        public:
//            std::unique_ptr<ftgl::texture_atlas_t, decltype(&ftgl::texture_atlas_delete)> m_FTAtlas;
//            std::unique_ptr<ftgl::texture_font_t, decltype(&ftgl::texture_font_delete)> m_FTFont;
//            ftgl::texture_atlas_t * m_FTAtlas;
//            ftgl::texture_font_t * m_FTFont;


        public:
            Layer(std::unique_ptr<BatchRenderer2D>, std::unique_ptr<Shader>, const math::mat4 &);

            ~Layer() = default;

            void renderSprites(const entities::World &world);

            void renderTexturedSprites(const entities::World &world);

            void renderText(const entities::World &world);

            Shader &getShader() { return *m_Shader; }

            void begin() const;

            void end() const;

            /**
             * This function should always return a unique ID.
             *
             * @return unique ID
             */
            GLuint getLayerID() { return m_Shader->getProgram(); }

            void setFontManager(std::unique_ptr<graphics::FontManager> fontManager) {
                m_FontManager = std::move(fontManager);
            }

            const graphics::FontManager & getFontManager(){
                return *m_FontManager;
            }

        public:
            /**
             * A tile layer is a layer with top down projection matrix.
             * @param maxSpriteCount tile layer uses batch rendering, specify maximum number of sprite that the layer will
             * @param vertexShader path to shader
             * @param fragmentShader path to shader
             * hold.
             * @return
             */
            static std::unique_ptr<Layer>
            createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                            std::string &&fragmentShader);

            /**
             * Similar to tile layer but rendered with textures instead of solid colors.
             * @param maxSpriteCount
             * @return
             */
            static std::unique_ptr<Layer>
            createTexturedTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                    std::string &&fragmentShader);

            /**
             * Similar to tile layer but for rendering texts.
             * @param maxSpriteCount
             * @return
             */
            static std::unique_ptr<Layer>
            createTextTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                std::string &&fragmentShader);

        };


    }
}