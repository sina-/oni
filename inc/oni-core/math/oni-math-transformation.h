#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/entities/oni-entities-fwd.h>


namespace oni {
    Rectangle
    shapeTransformation(const mat4 &,
                        const Rectangle &);

    mat4
    createTransformation(const WorldP3D &position,
                         const Orientation &rotation);

    mat4
    createTransformation(const WorldP3D &position,
                         const Orientation &rotation,
                         const Scale &scale);

    void
    worldToLocalTranslation(const WorldP3D &reference,
                            WorldP3D &operand);

    void
    localToWorldTranslation(const WorldP3D &reference,
                            vec3 &operand);

    void
    localToWorldTranslation(r32 x,
                            r32 y,
                            vec3 &operand);

    void
    localToWorldTranslation(const WorldP3D &,
                            Rectangle &);

    WorldP3D
    localToWorldTranslation(const mat4 &trans,
                            const WorldP3D &operand);

    void
    localToTextureTranslation(r32 ratio,
                              WorldP3D &operand);

    /**
     * Translates operand in the world coordinates to local coordinates of reference in texture coordinates.
     * For example, if object A is at (5, 10, 0) in the world, and object B at (1, 1, 0), given that
     * each unit of distance in game occupies 20 units of pixel in a texture, that is ratio, then point B
     * is located at (1 - 5, 10 - 1, 0 - 0) in reference to A in game units and multiply that
     * by the ratio to get the texture coordinates.
     *
     * @param reference to which the translation is applied in world coordinates
     * @param ratio each game unit is worth this many texture pixels
     * @param operand input as world coordinate and outputs as local texture coordinates
     */
    void
    worldToTextureCoordinate(const oni::WorldP3D &reference,
                             r32 ratio,
                             oni::WorldP3D &operand);
}
