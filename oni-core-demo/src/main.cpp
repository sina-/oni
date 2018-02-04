#include <iostream>
#include <graphics/batchrenderer2d.h>
#include <graphics/shader.h>
#include <graphics/window.h>
#include <chrono>
#include <graphics/tilelayer.h>

int main() {

    using namespace oni;
    using namespace graphics;
    using namespace math;
    using namespace std;
    using namespace buffers;

    int width = 1600;
    int height = 900;

    // NOTE: any call to GLEW functions will fail with Segfault if GLFW is uninitialized (initialization happens in Window).
    Window window("Oni Demo", width, height);

    auto drawLayerShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");
    auto backGroundLayerShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");

    auto lightLayerShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/spotlight.frag");

    auto drawLayer = std::make_unique<TileLayer>(std::move(drawLayerShader), 10000);
    auto backGroundLayer = std::make_unique<TileLayer>(std::move(backGroundLayerShader), 500000);
    auto lightLayer = std::make_unique<TileLayer>(std::move(lightLayerShader), 10);
    lightLayer->add(make_unique<Renderable2D>(math::vec2(16.0f, 9.0f), math::vec3(0.0f, 0.0f, 0.0f),
                                                           math::vec4(1, 1, 1, 0)));

    srand(static_cast<unsigned int>(time(nullptr)));

    float yStep = 0.06f;
    float xStep = 0.04f;

    for (float y = 0.5; y < 8.5f; y += yStep) {
        for (float x = 0.5; x < 15.5f; x += xStep) {
            backGroundLayer->add(make_unique<Renderable2D>(math::vec2(xStep, yStep), math::vec3(x, y, 0.0f),
                                                           math::vec4(rand() % 1000 / 1000.0f, 0, 1, 1)));
        }
    }
    float frameTime = 0.0f;

    while (!window.closed()) {
        auto start = std::chrono::steady_clock::now();

        window.clear();

        const auto mouseX = window.getCursorX();
        const auto mouseY = window.getCursorY();

        width = window.getWidth();
        height = window.getHeight();


        if (window.getMouseButton() != GLFW_KEY_UNKNOWN) {
            drawLayer->add(
                    std::move(make_unique<Renderable2D>(math::vec2(0.05f, 0.07f),
                                                        math::vec3(((const float) mouseX) * 16.0f / width,
                                                                   9.0f - ((const float) mouseY) * 9.0f / height, 0.0f),
                                                        math::vec4(rand() % 1000 / 1000.0f, 0, 0, 1))));
        }


        const auto & lightShader = lightLayer->getShader();
        lightShader->enable();
        lightShader->setUniform2f("light_pos", vec2(static_cast<float>(mouseX * 16.0f / width),
                                                    static_cast<float>(9.0f - mouseY * 9.0f / height)));
        lightShader->disable();

        lightLayer->render();
        backGroundLayer->render();
        drawLayer->render();


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