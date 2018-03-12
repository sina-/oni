#include <iostream>
#include <chrono>

#include <graphics/batch-renderer-2d.h>
#include <graphics/window.h>
#include <graphics/texture.h>
#include <graphics/layer.h>
#include <graphics/light.h>
#include <physics/movement.h>

int main() {
    using namespace oni;
    int width = 1600;
    int height = 900;

    srand(static_cast<unsigned int>(time(nullptr)));

    entities::World world;
    physics::Movement movement;

    // NOTE: any call to GLEW functions will fail with Segfault if GLEW is uninitialized (initialization happens in Window).
    graphics::Window window("Oni Demo", width, height);

    auto fontManager = std::make_unique<graphics::FontManager>("resources/fonts/FreeMonoBold.ttf", 20);

    auto spriteLayer = graphics::Layer::createTexturedTileLayer(500000, "shaders/texture.vert", "shaders/texture.frag");
    auto particleLayer = graphics::Layer::createTileLayer(10000, "shaders/basic.vert", "shaders/basic.frag");
    auto lightLayer = graphics::Layer::createTileLayer(10, "shaders/basic.vert", "shaders/spotlight.frag");
    auto carLayer = graphics::Layer::createTexturedTileLayer(10, "shaders/texture.vert", "shaders/texture.frag");
    auto textLayer = graphics::Layer::createTexturedTileLayer(100, "shaders/text.vert", "shaders/text.frag");

    float yStep = 0.6f;
    float xStep = 0.4f;

    float xStart = 0.5f;
    float xEnd = 15.5f;

    float yStart = 0.5f;
    float yEnd = 8.5f;

    auto batchSize = static_cast<unsigned long>(((yEnd - yStart + 1) / yStep) * ((xEnd - xStart + 1) / xStep));
    world.reserveEntity(batchSize);

    auto spriteTexture = graphics::LoadTexture::load("resources/images/test.png");
    auto spriteTexture2 = graphics::LoadTexture::load("resources/images/test2.png");
    auto spriteTexture3 = graphics::LoadTexture::load("resources/images/test3.png");

    auto spriteLayerID = components::LayerID(spriteLayer->getLayerID());

    for (float y = yStart; y < yEnd; y += yStep) {
        for (float x = xStart; x < xEnd; x += xStep) {
            auto sprite = world.createEntity(entities::TexturedSprite, spriteLayerID);

            auto spritePlacement = components::Placement();
            spritePlacement.vertexA = math::vec3(x, y, 0.0f);
            spritePlacement.vertexB = math::vec3(x, y + yStep, 0.0f);
            spritePlacement.vertexC = math::vec3(x + xStep, y + yStep, 0.0f);
            spritePlacement.vertexD = math::vec3(x + xStep, y, 0.0f);

            auto spriteAppearance = components::Appearance();
            spriteAppearance.color = math::vec4(rand() % 1000 / 1000.0f, 0, 1, 1);

            world.setEntityPlacement(sprite, spritePlacement);
            world.setEntityAppearance(sprite, spriteAppearance);
            auto i = rand() % 3;
            if (i == 0) {
                world.setEntityTexture(sprite, spriteTexture);
            } else if (i == 1) {
                world.setEntityTexture(sprite, spriteTexture2);
            } else {
                world.setEntityTexture(sprite, spriteTexture3);
            }
        }
    }

    auto carLayerID = components::LayerID(carLayer->getLayerID());
    auto car = world.createEntity(entities::DynamicTexturedSprite, carLayerID);

    auto carPlacement = components::Placement();
    carPlacement.vertexA = math::vec3(0.0f, 0.0f, 0.0f);
    carPlacement.vertexB = math::vec3(0.0f, 3.0f, 0.0f);
    carPlacement.vertexC = math::vec3(1.0f, 3.0f, 0.0f);
    carPlacement.vertexD = math::vec3(1.0f, 0.0f, 0.0f);

    auto carVelocity = components::Velocity();
    carVelocity.magnitude = 0.0005f;
    carVelocity.direction = math::vec3();

    auto carTexture = graphics::LoadTexture::load("resources/images/test.png");

    world.setEntityPlacement(car, carPlacement);
    world.setEntityVelocity(car, carVelocity);
    world.setEntityTexture(car, carTexture);

    auto lightLayerID = components::LayerID(lightLayer->getLayerID());

    auto light = world.createEntity(entities::Sprite, lightLayerID);

    auto lightPlacement = components::Placement();
    lightPlacement.vertexA = math::vec3(0.0f, 0.0f, 0.0f);
    lightPlacement.vertexB = math::vec3(0.0f, 9.0f, 0.0f);
    lightPlacement.vertexC = math::vec3(16.0f, 9.0f, 0.0f);
    lightPlacement.vertexD = math::vec3(16.0f, 0.0f, 0.0f);

    auto lightAppearance = components::Appearance();
    lightAppearance.color = math::vec4(0, 0.6f, 0.5f, 0.0f);

    world.setEntityPlacement(light, lightPlacement);
    world.setEntityAppearance(light, lightAppearance);

    auto textLayerID = components::LayerID(textLayer->getLayerID());
    auto text = world.createEntity(entities::TextSprite, textLayerID);

    auto textString = fontManager->createTextFromString("HELLO BASHTARDI MY OLD FRIEND", math::vec3(0.0f, 0.0f, 1.0f));

    world.setEntityText(text, textString);

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
            auto particleLayerID = components::LayerID(particleLayer->getLayerID());
            auto particle = world.createEntity(entities::Sprite, particleLayerID);

            auto particlePlacement = components::Placement();
            particlePlacement.vertexA = math::vec3(_x, _y, 0.0f);
            particlePlacement.vertexB = math::vec3(_x, _y + 0.07f, 0.0f);
            particlePlacement.vertexC = math::vec3(_x + 0.05f, _y + 0.07f, 0.0f);
            particlePlacement.vertexD = math::vec3(_x + 0.05f, _y, 0.0f);

            auto particleAppearance = components::Appearance();
            particleAppearance.color = math::vec4(rand() % 1000 / 1000.0f, 0, 0, 1);

            world.setEntityPlacement(particle, particlePlacement);
            world.setEntityAppearance(particle, particleAppearance);
        }

        movement.update(world, window);
        graphics::Light::update(*lightLayer, mouseX, mouseY, width, height);

        spriteLayer->renderTexturedSprites(world);
        carLayer->renderTexturedSprites(world);
        particleLayer->renderSprites(world);
        lightLayer->renderSprites(world);
        textLayer->renderText(world);

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