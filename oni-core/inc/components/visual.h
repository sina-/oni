#pragma once

#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>

#include <GL/glew.h>
#include <memory>
#include <utility>

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

        // TODO: Does this have to be a struct? I think it can be just saved as GLuint with two copies, one
        // in the world and another in the layer.
        struct LayerID {
            /** Determines the Layer. And that effectively clusters entities based on
             * Layer and helps renderer to only switch state per cluster of entities.
             */
            GLuint layerID;

            LayerID() : layerID(0) {}

            explicit LayerID(GLuint _id) : layerID(_id) {}

            LayerID(const LayerID &) = default;
        };

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
            std::string text;

            Text() : text(std::string()) {}

            explicit Text(std::string _text) : text(std::move(_text)) {}
        };

    }
}