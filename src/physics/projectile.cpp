#include <oni-core/physics/projectile.h>

#include <Box2D/Box2D.h>
#include <GLFW/glfw3.h>

#include <oni-core/entities/client-data-manager.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/component/geometry.h>
#include <oni-core/entities/create-entity.h>

namespace oni {
    namespace physics {
        Projectile::Projectile(b2World *physicsWorld) : mPhysicsWorld{physicsWorld} {

        }

        Projectile::~Projectile() = default;

        void Projectile::tick(entities::EntityManager &manager,
                              entities::ClientDataManager &clientData,
                              common::real64 tickTime) {
            auto carView = manager.createViewScopeLock<component::Placement, component::Car, component::Tag_Vehicle>();

            for (auto &&entity: carView) {
                auto clientLock = clientData.scopedLock();
                const auto &input = clientData.getClientInput(entity);

                if (input.isPressed(GLFW_KEY_F)) {
                    auto &carPlacement = carView.get<component::Placement>(entity);
                    auto &carPos = carPlacement.position;
                    auto &heading = carView.get<component::Car>(entity).heading;
                    auto bulletID = createBullet(manager, carPos);
                    auto *body = manager.get<component::PhysicalProperties>(bulletID).body;
                    body->ApplyForceToCenter(
                            b2Vec2(static_cast<common::real32>(std::cos(heading) * 200),
                                   static_cast<common::real32>(std::sin(heading) * 200)),
                            true);
                }
            }
        }

        common::EntityID Projectile::createBullet(entities::EntityManager &manager, const math::vec3 &pos) {
            math::vec3 bulletPos{pos.x, pos.y, pos.z};
            math::vec2 bulletSize{0.3f, 0.1f};

            std::string bulletTexture = "resources/images/bullet/1.png";

            auto bulletID = oni::entities::createEntity(manager);
            oni::entities::assignPhysicalProperties(manager, *mPhysicsWorld, bulletID,
                                                    bulletPos,
                                                    bulletSize,
                                                    oni::component::BodyType::DYNAMIC, true);
            oni::entities::assignShapeLocal(manager, bulletID, bulletSize, pos.z);
            oni::entities::assignPlacement(manager, bulletID, bulletPos, math::vec3{1.f, 1.f, 0.f},
                                           0.f);
            oni::entities::assignTextureToLoad(manager, bulletID, bulletTexture);
            oni::entities::assignTag<component::Tag_Dynamic>(manager, bulletID);

            return bulletID;
        }
    }
}