#pragma once

#include <GL/glew.h>
#include <stdexcept>

#define CHECK_OGL_ERRORS { GLenum __ogl_error_; \
    while ((__ogl_error_ = glGetError()) != GL_NO_ERROR) { \
        throw std::runtime_error("OpenGL Error: " + std::string(reinterpret_cast<const char *>(gluErrorString(__ogl_error_)))); \
    }}
