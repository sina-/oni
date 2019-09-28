#include <oni-core/physics/oni-physics-system.h>

#include <oni-core/io/oni-io-input-data.h>
#include <oni-core/entities/oni-entities-client-data-manager.h>

#include <GLFW/glfw3.h>

namespace oni {
    System_CarInput::System_CarInput(EntityManager &em,
                                     ClientDataManager &cdm) : SystemTemplate(em), mClientDataMng(cdm) {}

    void
    System_CarInput::update(EntityTickContext &etc,
                            CarInput &carInput) {
        constexpr r32 steeringSensitivity = 0.9f;
        auto input = mClientDataMng.getClientInput(etc.id);
        if (!input) {
            return;
        }

        carInput = {};

        // TODO: This should not be exposed by the input class!
        if (input->isPressed(GLFW_KEY_W) || input->isPressed(GLFW_KEY_UP)) {
            // TODO: When using game-pad, this value will vary between (0.0f...1.0f)
            carInput.throttle = 1.f;
        }
        if (input->isPressed(GLFW_KEY_A) || input->isPressed(GLFW_KEY_LEFT)) {
            carInput.left = steeringSensitivity;
        }
        if (input->isPressed(GLFW_KEY_S) || input->isPressed(GLFW_KEY_DOWN)) {
            carInput.throttle = -1.f;
        }
        if (input->isPressed(GLFW_KEY_D) || input->isPressed(GLFW_KEY_RIGHT)) {
            carInput.right = steeringSensitivity;
        }
        if (input->isPressed(GLFW_KEY_LEFT_SHIFT)) {
            carInput.nitro = true;
        }
        if (input->isPressed(GLFW_KEY_SPACE)) {
            carInput.eBrake = 1.f;
        }
    }

    void
    System_CarInput::postUpdate(EntityManager &mng,
                                duration32 dt) {
    }
}