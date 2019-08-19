#include <oni-core/graphic/oni-graphic-debug-draw-box2d.h>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/graphic/oni-graphic-scene-manager.h>


namespace oni {
    DebugDrawBox2D::DebugDrawBox2D(SceneManager *sceneManager) {
        mSceneManager = sceneManager;
    }

    DebugDrawBox2D::~DebugDrawBox2D() = default;

    void
    DebugDrawBox2D::DrawPolygon(const b2Vec2 *vertices,
                                int32 vertexCount,
                                const b2Color &c) {
        if (vertexCount == 4) {
            auto color = Color{};
            color.set_rgba(c.r, c.g, c.b, c.a);
            auto pos = WorldP3D{vertices->x, vertices->y, 1.f};
            mSceneManager->renderRaw(pos, color);
        }
    }

    void
    DebugDrawBox2D::DrawSolidPolygon(const b2Vec2 *vertices,
                                     int32 vertexCount,
                                     const b2Color &c) {
        if (vertexCount == 4) {
            auto pos = WorldP3D{vertices->x, vertices->y, 1.f};
            auto color = Color{};
            color.set_rgba(c.r, c.g, c.b, c.a);
            mSceneManager->renderRaw(pos, color);
        }
    }

    void
    DebugDrawBox2D::DrawCircle(const b2Vec2 &center,
                               float32 radius,
                               const b2Color &color) {

    }

    void
    DebugDrawBox2D::DrawSolidCircle(const b2Vec2 &center,
                                    float32 radius,
                                    const b2Vec2 &axis,
                                    const b2Color &color) {

    }

    void
    DebugDrawBox2D::DrawSegment(const b2Vec2 &p1,
                                const b2Vec2 &p2,
                                const b2Color &color) {

    }

    void
    DebugDrawBox2D::DrawTransform(const b2Transform &xf) {

    }

    void
    DebugDrawBox2D::DrawPoint(const b2Vec2 &p,
                              float32 size,
                              const b2Color &color) {

    }

    void
    DebugDrawBox2D::Begin() {
        mSceneManager->beginColorRendering();
    }

    void
    DebugDrawBox2D::End() {
        mSceneManager->endColorRendering();
    }
}