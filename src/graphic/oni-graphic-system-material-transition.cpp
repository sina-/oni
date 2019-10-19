#include <oni-core/graphic/oni-graphic-system.h>

namespace oni {
    System_MaterialTransition::System_MaterialTransition(EntityManager&em) : SystemTemplate(em) {}

    void
    System_MaterialTransition::updateTextureAnimated(MaterialTransition_Texture &mta,
                                                     r64 dt) {
        mta.ttl.currentAge += dt;
        // NOTE: It is assumed that this function is called more often than animation fps!
        assert(mta.ttl.maxAge > dt);
        if (mta.playing && almost_Greater(mta.ttl.currentAge, mta.ttl.maxAge)) {
            mta.nextFrame = (mta.nextFrame + 1) % enumCast(mta.numFrames);
            mta.ttl.currentAge = 0;
        }
    }

    void
    System_MaterialTransition::updateTint(Material_Skin &skin,
                                          MaterialTransition_Color mtc,
                                          const TimeToLive &ttl) {
        // TODO: Very accurate and slow calculations, I don't need the accuracy but it can be faster!
        auto &begin = mtc.begin;
        auto &end = mtc.end;
        auto t = ttl.currentAge / ttl.maxAge;

        auto r = lerp(begin.r_r32(), end.r_r32(), t);
        auto g = lerp(begin.g_r32(), end.g_r32(), t);
        auto b = lerp(begin.b_r32(), end.b_r32(), t);
        auto a = lerp(begin.a_r32(), end.a_r32(), t);

        skin.color.set_r(r);
        skin.color.set_g(g);
        skin.color.set_b(b);
        skin.color.set_a(a);
    }

    void
    System_MaterialTransition::updateFade(Material_Skin &skin,
                                          MaterialTransition_Fade &fade,
                                          const TimeToLive &ttl) {
        auto targetAlpha = 1.f;
        switch (fade.fadeFunc) {
            case FadeFunc::LINEAR: {
                targetAlpha = 1 - ttl.currentAge / ttl.maxAge;
                break;
            }
            case FadeFunc::TAIL: {
                constexpr auto dropOffT = 0.7f;
                auto ageRatio = ttl.currentAge / ttl.maxAge;
                if (ageRatio > dropOffT) {
                    targetAlpha = 1 - ageRatio;
                }
                break;
            }
            default: {
                assert(false);
                break;
            }
        }

        auto currentAlpha = skin.color.a_r32();
        skin.color.set_a(lerp(currentAlpha, targetAlpha, fade.factor));
    }

    void
    System_MaterialTransition::update(EntityTickContext &etc,
                                      MaterialTransition_List &mtl,
                                      Material_Skin &ms) {
        if (mtl.ended) {
            return;
        }
        auto &current = mtl.transitions[mtl.activeTransIdx];
        current.ttl.currentAge += etc.dt;
        if (almost_Greater(current.ttl.currentAge, current.ttl.maxAge)) {
            if (mtl.transitions.size() - 1 > mtl.activeTransIdx) {
                current.ttl.currentAge = 0.f;
                ++mtl.activeTransIdx;
            } else {
                switch (mtl.ending) {
                    case MaterialTransition_EndBehavior::LOOP: {
                        current.ttl.currentAge = 0.f;
                        mtl.activeTransIdx = 0;
                        break;
                    }
                    case MaterialTransition_EndBehavior::PLAY_AND_STOP: {
                        // TODO: Can I not have this special case?
                        if (current.type == MaterialTransition_Type::TEXTURE) {
                            current.texture.playing = false;
                        }
                        mtl.ended = true;
                        break;
                    }
                    case MaterialTransition_EndBehavior::PLAY_AND_REMOVE_ENTITY: {
                        assert(etc.mng.getSimMode() != SimMode::CLIENT_SIDE_SERVER);
                        etc.mng.markForDeletion(etc.id);
                        break;
                    }
                    default: {
                        assert(false);
                    }
                }
            }
        } else {
            switch (current.type) {
                case MaterialTransition_Type::TEXTURE: {
                    updateTextureAnimated(current.texture, etc.dt);
                    break;
                }
                case MaterialTransition_Type::FADE: {
                    updateFade(ms, current.fade, current.ttl);
                    break;
                }
                case MaterialTransition_Type::TINT: {
                    updateTint(ms, current.color, current.ttl);
                    break;
                }
                default: {
                    assert(false);
                }
            }
        }
    }

    void
    System_MaterialTransition::postUpdate(EntityManager &mng,
                                          duration32 dt) {

    }
}