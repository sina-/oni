#include <oni-core/graphics/debug-draw-box2d.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/components/geometry.h>
#include <oni-core/graphics/scene-manager.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/texture-manager.h>


namespace oni {
    namespace graphics {

        DebugDrawBox2D::DebugDrawBox2D(graphics::SceneManager *sceneManager) {
            mSceneManager = sceneManager;
        }

        DebugDrawBox2D::~DebugDrawBox2D() = default;

        void DebugDrawBox2D::DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
            if (vertexCount == 4) {
                auto currentVertex = *vertices;
                auto a = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto b = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto c = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto d = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                auto shape = components::Shape{a, b, c, d};

                auto appearance = components::Appearance{math::vec4{1.0f, 0.0f, 0.0f, 0.3f}};
                mSceneManager->renderRaw(shape, appearance);
            }
        }

        void DebugDrawBox2D::DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color) {
            if (vertexCount == 4) {
                auto currentVertex = *vertices;
                auto a = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto b = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto c = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                currentVertex = *(++vertices);
                auto d = math::vec3{currentVertex.x, currentVertex.y, 1.0f};
                auto shape = components::Shape{a, b, c, d};

                auto appearance = components::Appearance{math::vec4{1.0f, 0.0f, 0.0f, 0.3f}};
                mSceneManager->renderRaw(shape, appearance);
            }
        }

        void DebugDrawBox2D::DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &color) {

        }

        void DebugDrawBox2D::DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis,
                                             const b2Color &color) {

        }

        void DebugDrawBox2D::DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color) {

        }

        void DebugDrawBox2D::DrawTransform(const b2Transform &xf) {

        }

        void DebugDrawBox2D::DrawPoint(const b2Vec2 &p, float32 size, const b2Color &color) {

        }

        void DebugDrawBox2D::Begin() {
            mSceneManager->beginColorRendering();
        }

        void DebugDrawBox2D::End() {
            mSceneManager->endColorRendering();
        }
    }
}