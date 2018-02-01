#include <iostream>
#include <graphics/batchrenderer2d.h>
#include <graphics/shader.h>
#include <graphics/staticsprite.h>
#include <graphics/window.h>
#include <chrono>

int main() {

    using namespace oni;
    using namespace graphics;
    using namespace math;
    using namespace std;
    using namespace buffers;

    int width = 960;
    int height = 540;

    // NOTE: any call to GLEW functions will fail with Segfault if GLFW is uninitialized (initialization happens in Window).
    Window window("Oni Demo", width, height);

    Shader shader("shaders/basic.vert", "shaders/basic.frag");
    shader.enable();
//    shader.setUniformMat4("vw_matrix", mat4::translation(vec3(2, 2, 0)));
    shader.setUniformMat4("pr_matrix", mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f));

//    shader.setUniform4f("input_color", vec4(0.2f, 0.3f, 0.8f, 1.0f));
//    shader.setUniform2f("light_pos", vec2(4.0f, 1.5f));

    auto sprite1 = std::make_unique<Renderable2D>(math::vec2(4, 4), math::vec3(5, 5, 0), math::vec4(1, 0, 1, 1));
    auto sprite2 = std::make_unique<Renderable2D>(math::vec2(1, 2), math::vec3(0, 1, 1), math::vec4(0, 1, 1, 1));

    srand(static_cast<unsigned int>(time(nullptr)));

    std::vector<unique_ptr<Renderable2D>> sprites;

    float yStep = 0.05f;
    float xStep = 0.02f;

    for (float y = 1; y < 8.0f; y += yStep) {
        for (float x = 1; x < 15.0f; x += xStep) {
            sprites.push_back(
                    make_unique<Renderable2D>(math::vec2(xStep, yStep), math::vec3(x, y, 0.0f),
                                              math::vec4(rand() % 1000 / 1000.0f, 0, 1, 1)));
        }
    }

    auto renderer = std::make_unique<BatchRenderer2D>(sprites.size() + 1);
    float frameTime = 0.0f;

    while (!window.closed()) {
        auto start = std::chrono::steady_clock::now();

        window.clear();

        auto mouseX = window.getCursorX();
        auto mouseY = window.getCursorY();

        width = window.getWidth();
        height = window.getHeight();

        shader.setUniform2f("light_pos", vec2(static_cast<float>(mouseX * 16.0f / width),
                                              static_cast<float>(9.0f - mouseY * 9.0f / height)));

        renderer->begin();
        for (const auto &sprite: sprites) {
            renderer->submit(sprite);
        }
        renderer->end();
        renderer->flush();
        window.update();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float> diff = end - start;
        frameTime += diff.count();
        if (frameTime > 1.0f) {
            printl(1.0f / diff.count());
            frameTime = 0;
        }
    }

    return 0;
}