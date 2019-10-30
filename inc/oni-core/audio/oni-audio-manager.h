#pragma once

#include <vector>
#include <memory>
#include <map>
#include <unordered_map>

#include <oni-core/audio/oni-audio-fwd.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/game/oni-game-fwd.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/asset/oni-asset-manager.h>


namespace FMOD {
    class Sound;

    class System;

    class Channel;

    class ChannelGroup;
}

namespace oni {
    class AudioManager {
    public:
        explicit AudioManager(AssetFilesIndex &);

        void
        tick(const WorldP3D &playerPos);

        void
        playCollisionSoundEffect(const Event_Collision &);

        void
        kill(EntityID);

        void
        playOneShot(const Sound &,
                    const SoundPitch &,
                    const vec3 &distance);

        void
        setChannelGroupVolume(ChannelGroup,
                              r32 volume);

        r32
        getChannelGroupVolume(ChannelGroup);

        void
        setMasterVolume(r32 volume);

        friend System_PlayPauseSound;

    private:
        class FMODDeleter {
        public:
            void
            operator()(FMOD::Sound *s) const;

            void
            operator()(FMOD::System *sys) const;

            void
            operator()(FMOD::ChannelGroup *channel) const;
        };

        using CollisionSoundTag = u32p;
        using EntitySoundTag = u32p;

        struct EntityChannel {
            EntityID entityID{0};
            FMOD::Channel *channel{nullptr};
        };

    private:
        void
        preLoadSounds();

        static CollisionSoundTag
        createCollisionEffectID(const PhysicalCatPair &);

        FMOD::Channel *
        createChannel(const Sound &);

        FMOD::ChannelGroup *
        getChannelGroup(ChannelGroup);

        void
        loadSound(Sound_Tag,
                  std::string_view);

        void
        loadChannels();

        static EntitySoundTag
        createEntitySoundID(Sound_Tag,
                            EntityID);

        AudioManager::EntityChannel &
        getOrCreateLooping3DChannel(const Sound &,
                                    EntityID);

        void
        setPitch(FMOD::Channel &,
                 r32 pitch);

        void
        set3DPos(FMOD::Channel &,
                 const vec3 &pos,
                 const vec3 &velocity);

        bool
        isPaused(FMOD::Channel &);

        void
        unPause(FMOD::Channel &);

        void
        pause(FMOD::Channel &);

    private:
        AssetFilesIndex &mAssetManager;
        std::unique_ptr<FMOD::System, FMODDeleter> mSystem;

        std::unordered_map<ChannelGroup, std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>> mChannelGroup;
        std::unordered_map<Sound_Tag, std::unique_ptr<FMOD::Sound, FMODDeleter>> mSounds;
        std::unordered_map<AudioManager::EntitySoundTag, EntityChannel> mLoopingChannels;
        std::unordered_map<AudioManager::CollisionSoundTag, Sound_Tag> mCollisionEffects;
        std::unordered_map<ChannelGroup, r32> mChannelVolume;

        r32 mMasterVolume{1.f};

        WorldP3D mPlayerPos{};

        r32 mMaxAudibleDistance{0.f};
        i32 mMaxNumberOfChannels{0};
    };
}