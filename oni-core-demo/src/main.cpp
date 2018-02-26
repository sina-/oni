#include <iostream>
#include <chrono>

#include <graphics/batch-renderer-2d.h>
#include <graphics/window.h>
#include <graphics/tilelayer.h>
#include <graphics/texture.h>
#include <physics/movement.h>
#include <components/physical.h>

int main() {
    using namespace oni;
    using namespace graphics;
    using namespace math;
    using namespace std;
    using namespace buffers;
    using namespace components;

    int width = 1600;
    int height = 900;

    entities::World world;
    physics::Movement movement;

    // NOTE: any call to GLEW functions will fail with Segfault if GLEW is uninitialized (initialization happens in Window).
    Window window("Oni Demo", width, height);

    auto spriteShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/basic.frag");

    auto lightShader = std::make_unique<Shader>("shaders/basic.vert", "shaders/spotlight.frag");

    auto spriteLayer = std::make_unique<TileLayer>(std::move(spriteShader), 500000);
    auto lightLayer = std::make_unique<TileLayer>(std::move(lightShader), 10);

    srand(static_cast<unsigned int>(time(nullptr)));

    float yStep = 0.06f;
    float xStep = 0.04f;

    float xStart = 0.5f;
    float xEnd = 15.5f;

    float yStart = 0.5f;
    float yEnd = 8.5f;

    auto batchSize = static_cast<unsigned long>(((yEnd - yStart + 1) / yStep) * ((xEnd - xStart + 1) / xStep));
    world.reserveEntity(batchSize);

    for (float y = yStart; y < yEnd; y += yStep) {
        for (float x = xStart; x < xEnd; x += xStep) {
            auto sprite = world.createEntity(entities::Sprite);

            auto spritePlacement = Placement();
            spritePlacement.vertexA = vec3(x, y, 0.0f);
            spritePlacement.vertexB = vec3(x, y + yStep, 0.0f);
            spritePlacement.vertexC = vec3(x + xStep, y + yStep, 0.0f);
            spritePlacement.vertexD = vec3(x + xStep, y, 0.0f);

            auto spriteAppearance = Appearance();
            spriteAppearance.color = math::vec4(rand() % 1000 / 1000.0f, 0, 1, 1);

            world.setEntityPlacement(sprite, spritePlacement);
            world.setEntityAppearance(sprite, spriteAppearance);
        }
    }

    auto car = world.createEntity(entities::DynamicSprite);

    auto carPlacement = Placement();
    carPlacement.vertexA = vec3(0.0f, 0.0f, 0.0f);
    carPlacement.vertexB = vec3(0.0f, 3.0f, 0.0f);
    carPlacement.vertexC = vec3(1.0f, 3.0f, 0.0f);
    carPlacement.vertexD = vec3(1.0f, 0.0f, 0.0f);

    auto carAppearance = Appearance();
    carAppearance.color = vec4(0.3f, 0.1f, 0.2f, 1.0f);

    auto carVelocity = Velocity();
    carVelocity.magnitude = 0.0005f;
    carVelocity.direction = vec3();

    world.setEntityPlacement(car, carPlacement);
    world.setEntityAppearance(car, carAppearance);
    world.setEntityVelocity(car, carVelocity);

    auto lightPlacement = Placement();
    lightPlacement.vertexA = vec3(1.0f, 1.0f, 0.0f);
    lightPlacement.vertexB = vec3(1.0f, 8.0f, 0.0f);
    lightPlacement.vertexC = vec3(15.0f, 8.0f, 0.0f);
    lightPlacement.vertexD = vec3(15.0f, 1.0f, 0.0f);
    auto lightAppearance = Appearance();
    lightAppearance.color = vec4(0, 0.6f, 0.5f, 0.0f);

    auto light = world.createEntity(components::LightningComponent);
    world.setEntityPlacement(light, lightPlacement);
    world.setEntityAppearance(light, lightAppearance);

    auto texture = LoadTexture::load("resources/images/test.png");
    auto textureEntity = world.createEntity(components::TextureComponent);
    world.setEntityTexture(textureEntity, texture);

    float frameTime = 0.0f;

    float totalFPS = 0.0f;
    int secondsPassed = 0;

    while (!window.closed()) {
        auto start = std::chrono::steady_clock::now();

        window.clear();

        const auto mouseX = window.getCursorX();
        const auto mouseY = window.getCursorY();

        width = window.getWidth();
        height = window.getHeight();

        // TODO: Creating new entities should be part of a new system
        if (window.getMouseButton() != GLFW_KEY_UNKNOWN) {
            auto _x = ((const float) mouseX) * 16.0f / width;
            auto _y = 9.0f - ((const float) mouseY) * 9.0f / height;
            auto sprite = world.createEntity(components::Mask().set(components::PLACEMENT).set(components::APPEARANCE));

            auto spritePlacement = Placement();
            spritePlacement.vertexA = vec3(_x, _y, 0.0f);
            spritePlacement.vertexB = vec3(_x, _y + 0.07f, 0.0f);
            spritePlacement.vertexC = vec3(_x + 0.05f, _y + 0.07f, 0.0f);
            spritePlacement.vertexD = vec3(_x + 0.05f, _y, 0.0f);

            auto spriteAppearance = Appearance();
            spriteAppearance.color = vec4(rand() % 1000 / 1000.0f, 0, 0, 1);

            world.setEntityPlacement(sprite, spritePlacement);
            world.setEntityAppearance(sprite, spriteAppearance);
        }

        // TODO: This update logic should be part of Layer or world if shader is to be a component.
        const auto &lightShader = lightLayer->getShader();
        lightShader->enable();
        lightShader->setUniform2f("light_pos", vec2(static_cast<float>(mouseX * 16.0f / width),
                                                    static_cast<float>(9.0f - mouseY * 9.0f / height)));
        lightShader->disable();

        // TODO: This is awkward way to handle different shaders. One way to solve the problem
        // is to make the shader a component of each entity, but that would require to switch
        // shader between rendering of each entity if render() is naive. A more sophisticated
        // rendered could batch the entities with based on shader type and render them together.
        // but then entities should be renderable regardless of their order.
        spriteLayer->render(world, components::AppearanceComponent);
        lightLayer->render(world, components::LightningComponent);

        movement.update(world, window);

        window.update();

        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float> diff = end - start;
        frameTime += diff.count();
        if (frameTime > 1.0f) {
            auto fps = 1.0f / diff.count();
            totalFPS += fps;
            secondsPassed++;
            printl(fps);
            frameTime = 0;
        }

    }
    printl("Average FPS: ");
    printl(totalFPS / secondsPassed);

    return 0;
}