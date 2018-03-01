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

        struct VertexData {
            oni::math::vec3 vertex;
            oni::math::vec4 color;
            oni::math::vec2 uv;
        };

        struct BufferStructure {
            GLuint index;
            GLuint componentCount;
            GLenum componentType;
            GLboolean normalized;
            GLsizei stride;
            const GLvoid *offset;

            BufferStructure(GLuint index, GLuint componentCount, GLenum componentType, GLboolean normalized,
                            GLsizei stride, const void *offset) : index(index), componentCount(componentCount),
                                                                  componentType(componentType), normalized(normalized),
                                                                  stride(stride), offset(offset) {}
        };

        typedef std::vector<std::unique_ptr<const BufferStructure>> BufferStructures;

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

        struct ShaderID {
            /* Determines which shader will render it. Which effectively clusters entities based on
             * shader and helps renderer to only switch shader per cluster of entities.
             */
            GLuint shaderID;

            ShaderID() : shaderID(0) {}

            explicit ShaderID(GLuint _id) : shaderID(_id) {}

            ShaderID(const ShaderID &) = default;
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

    }
}