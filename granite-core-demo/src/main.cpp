#include <iostream>
#include <graphics/batchrenderer2d.h>
#include "graphics/window.h"
#include "graphics/simple2drenderer.h"


int main() {

    using namespace granite;
    using namespace graphics;
    using namespace math;
    using namespace std;

    Window window("Granite", 800, 600);

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    shader.enable();
    shader.setUniformMat4("pr_matrix", mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));
    shader.setUniformMat4("vw_matrix", mat4::translation(vec3(2, 2, 0)));

    auto sprite1 = std::make_shared<Renderable2D>(math::vec2(4, 4), math::vec3(5, 5, 0), math::vec4(1, 0, 1, 1),
                                                  shader);
    auto sprite2 = std::make_shared<Renderable2D>(math::vec2(1, 2), math::vec3(0, 1, 1), math::vec4(0, 1, 1, 1),
                                                  shader);

    BatchRenderer2D renderer;

    auto renderer2D = std::make_unique<BatchRenderer2D>();

    while (!window.closed()) {
        window.clear();

        /*
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cout << "OpenGL error: " << err << std::endl;
        }
        */
        renderer.begin();
        renderer.submit(sprite1);
        renderer.submit(sprite2);
        renderer.end();
        renderer.flush();
        window.update();

    }

    return 0;
}