#include <iostream>
#include <graphics/batchrenderer2d.h>
#include <graphics/utils/checkoglerrors.h>
#include <graphics/shader.h>
#include <graphics/staticsprite.h>
#include "graphics/window.h"


int main() {

    using namespace oni;
    using namespace graphics;
    using namespace math;
    using namespace std;

    Window window("Oni Demo", 960, 540);

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    shader.enable();
//    shader.setUniformMat4("vw_matrix", mat4::translation(vec3(2, 2, 0)));
    shader.setUniformMat4("pr_matrix", mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));

//    shader.setUniform4f("input_color", vec4(0.2f, 0.3f, 0.8f, 1.0f));
    shader.setUniform2f("light_pos", vec2(4.0f, 1.5f));

/*
    auto sprite1 = std::make_shared<Renderable2D>(math::vec2(4, 4), math::vec3(5, 5, 0), math::vec4(1, 0, 1, 1));
    auto sprite2 = std::make_shared<Renderable2D>(math::vec2(1, 2), math::vec3(0, 1, 1), math::vec4(0, 1, 1, 1));
*/

    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<shared_ptr<Renderable2D>> sprites;
    for (float y = 0; y < 9.0f; y+= 0.2) {
        for (float x = 0; x < 16.0f; x+= 0.2) {
            sprites.push_back(
                    make_shared<StaticSprite>(math::vec2(0.9f, 0.9f), math::vec3(x, y, 0.0f),
                                              math::vec4(rand() % 1000 / 1000.0f, 0, 1, 1), shader));
        }
    }

    BatchRenderer2D renderer;

    auto renderer2D = std::make_unique<BatchRenderer2D>();

    while (!window.closed()) {
        window.clear();

        auto mouseX = window.getCursorX();
        auto mouseY = window.getCursorY();
        shader.setUniform2f("light_pos", vec2(static_cast<float>(mouseX * 16.0f / 960.0f), static_cast<float>(9.0f - mouseY * 9.0f / 540.0f)));

        /*
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL error: " << err << std::endl;
        }
        */
        CHECK_OGL_ERRORS
        renderer.begin();
        for (const auto &sprite: sprites) {
            renderer.submit(sprite);
        }
        CHECK_OGL_ERRORS
/*        renderer.submit(sprite1);
        renderer.submit(sprite2);*/
        renderer.end();
        renderer.flush();
        window.update();

    }

    return 0;
}