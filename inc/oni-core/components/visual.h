#pragma once

#include <memory>
#include <utility>

#include <GL/glew.h>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace components {
        // TODO: Switch to integer color. Kinda low prio as most of the time I'll use textured sprites.
        struct Appearance {
            math::vec4 color;

            Appearance() : color(math::vec4()) {}

            explicit Appearance(const math::vec4 &col) : color(col) {}

            Appearance(const Appearance &other) {
                color.x = other.color.x;
                color.y = other.color.y;
                color.z = other.color.z;
                color.w = other.color.w;
            }
        };

        /** Determines the Layer. And that effectively clusters entities based on
         * layer and helps renderer to only switch state per cluster of entities.
         */
        typedef GLuint LayerID;

        struct Texture {
            // TODO: This might need re ordering for better caching.
            GLsizei width;
            GLsizei height;
            GLuint textureID;
            std::string filePath;
            std::vector<math::vec2> uv;

            Texture() : filePath(std::string()), textureID(0), width(0), height(0), uv(std::vector<math::vec2>()) {};

            Texture(std::string _filePath, GLuint _textureID, GLsizei _width, GLsizei _height,
                    std::vector<math::vec2> _uv) : filePath(
                    std::move(_filePath)), textureID(_textureID), width(_width), height(_height), uv(std::move(_uv)) {};

            Texture(const Texture &other) {
                filePath = other.filePath;
                textureID = other.textureID;
                width = other.width;
                height = other.width;
                uv = other.uv;
            }
        };

        struct Text {
            // TODO: ordering?
            GLuint textureID;
            math::vec3 position;
            std::string textContent;
            std::vector<size_t> width;
            std::vector<size_t> height;
            std::vector<int> offsetX;
            std::vector<int> offsetY;
            std::vector<float> advanceX;
            std::vector<float> advanceY;
            std::vector<math::vec4> uv;
            float xScaling;
            float yScaling;

            Text() : textContent(std::string()), textureID(0), position(math::vec3()), uv(std::vector<math::vec4>()),
                     width(0), height(0), advanceX(0), advanceY(0), offsetX(0), offsetY(0), xScaling(1.0f),
                     yScaling(1.0f) {}
        };

    }
}