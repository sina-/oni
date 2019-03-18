#include <oni-core/audio/audio-manager.h>

#include <assert.h>
#include <oni-core/math/functions.h>

namespace oni {
    namespace audio {
        AudioManager::AudioManager() = default;

        void AudioManager::playCollisionSoundEffect(component::EntityType A, component::EntityType B) {
            auto soundID = createCollisionEffectID(A, B);
            assert(mCollisionEffects.find(soundID) != mCollisionEffects.end());
            play(mCollisionEffects[soundID]);
        }

        common::UInt16Pack AudioManager::createCollisionEffectID(component::EntityType A, component::EntityType B) {
            static_assert(sizeof(A) == sizeof(common::uint16), "Hashing will fail due to size mismatch");
            auto x = static_cast<common::uint16 >(A);
            auto y = static_cast<common::uint16 >(B);

            if (x > y) {
                std::swap(x, y); // Assuming soundEffect for A->B collision is same as B->A
            }

            auto soundID = math::packUInt16(x, y);
            return soundID;
        }

        void AudioManager::preLoadCollisionSoundEffects() {
            component::SoundID bulletWithUnknown = "resources/audio/collision/bullet-with-unknown.wav";
            loadSound(bulletWithUnknown);
            for (auto i = static_cast<common::uint16 >(component::EntityType::UNKNOWN);
                 i < static_cast<common::uint16>(component::EntityType::LAST);
                 ++i) {
                auto id = createCollisionEffectID(component::EntityType::SIMPLE_ROCKET,
                                                  static_cast<component::EntityType>(i));
                mCollisionEffects[id] = bulletWithUnknown;
            }
        }
    }
}