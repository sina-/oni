#include <oni-core/audio/oni-audio-manager.h>

#include <assert.h>

#include <fmod.hpp>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/game/oni-game-event.h>


#define ERRCHECK(_result) assert((_result) == FMOD_OK)

namespace oni {
    AudioManager::AudioManager(AssetFilesIndex &assetManager) : mAssetManager(assetManager) {
        mMaxAudibleDistance = 150.f;
        mMaxNumberOfChannels = 1024;

        FMOD::System *system;
        auto result = FMOD::System_Create(&system);
        ERRCHECK(result);
        mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

        u32 version;
        result = mSystem->getVersion(&version);
        ERRCHECK(result);

        assert(version >= FMOD_VERSION);

        result = mSystem->init(mMaxNumberOfChannels, FMOD_INIT_NORMAL, nullptr);
        ERRCHECK(result);

        result = mSystem->update();
        ERRCHECK(result);

        _loadChannels();
        _preLoadSounds();
    }

    void
    AudioManager::tick(const WorldP3D &playerPos) {
        mPlayerPos = playerPos;
        auto result = mSystem->update();
        ERRCHECK(result);
    }

    void
    AudioManager::playCollisionSoundEffect(const Event_Collision &event) {
        static_assert(sizeof(event.pcPair.a.id) == sizeof(i32), "Hashing will fail due to size mismatch");
        auto collisionTag = _createCollisionEffectID(event.pcPair);
        auto distance = mPlayerPos.value - event.pos.value;
        auto soundTag = mCollisionEffects[collisionTag];
        assert(soundTag.hash.value);
        auto sound = Sound{soundTag, ChannelGroup::EFFECT};
        auto pitch = SoundPitch{1.f};
        playOneShot(sound, pitch, distance);
    }

    void
    AudioManager::_preLoadSounds() {
        for (auto iter = mAssetManager.soundAssetsBegin(); iter != mAssetManager.soundAssetsEnd(); ++iter) {
            const auto &soundAsset = iter->second;
            _loadSound(soundAsset);
        }

        // TODO:
//        for (auto i = enumCast(PhysicalCategory::UNKNOWN);
//             i < enumCast(PhysicalCategory::LAST);
//             ++i) {
//            auto id = _createCollisionEffectID({PhysicalCategory::ROCKET,
//                                                static_cast<PhysicalCategory >(i)});
//            mCollisionEffects[id] = Sound_Tag::COLLISION_ROCKET_UNKNOWN;
//        }
    }

    AudioManager::CollisionSoundTag
    AudioManager::_createCollisionEffectID(const PhysicalCatPair &pcp) {
        auto a = pcp.a.id;
        auto b = pcp.b.id;

        if (a > b) {
            std::swap(a, b); // Assuming soundEffect for A->B collision is same as B->A
        }

        auto soundID = pack_i32(a, b);
        return soundID;
    }

    void
    AudioManager::_loadChannels() {
        FMOD::ChannelGroup *group{nullptr};
        auto result = mSystem->createChannelGroup("effectsChannel", &group);
        ERRCHECK(result);
        auto effectsGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
        effectsGroup->setVolume(1.f);
        mChannelGroup[ChannelGroup::EFFECT] = std::move(effectsGroup);
        group = nullptr;

        result = mSystem->createChannelGroup("musicChannel", &group);
        ERRCHECK(result);
        auto musicGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
        musicGroup->setVolume(1.f);
        mChannelGroup[ChannelGroup::MUSIC] = std::move(musicGroup);
        group = nullptr;

        for (auto i = enumCast(ChannelGroup::UNKNOWN) + 1;
             i < enumCast(ChannelGroup::LAST); ++i) {
            auto channelGroup = static_cast<ChannelGroup>(i);
            setChannelGroupVolume(channelGroup, 1.f);
        }
    }

    void
    AudioManager::kill(EntityID entityID) {
        for (auto it = mLoopingChannels.begin(); it != mLoopingChannels.end();) {
            if (it->second.entityID == entityID) {
                auto result = it->second.channel->stop();
                ERRCHECK(result);
                it = mLoopingChannels.erase(it);
            } else {
                ++it;
            }
        }
    }

    void
    AudioManager::_loadSound(const SoundAsset &asset) {
        FMOD::Sound *sound{};
        auto result = mSystem->createSound(asset.path.getFullPath().data(), FMOD_DEFAULT, nullptr, &sound);
        ERRCHECK(result);
        assert(sound);

        std::unique_ptr<FMOD::Sound, FMODDeleter> value(sound);
        mSounds.emplace(asset.name.hash, std::move(value));
    }

    FMOD::Channel *
    AudioManager::_createChannel(const Sound &sound) {
        assert(mChannelGroup[sound.group]);
        auto *group = mChannelGroup[sound.group].get();
        assert(group);
        assert(mSounds[sound.name.hash]);

        FMOD::Channel *channel{nullptr};
        auto paused = true;
        auto result = mSystem->playSound(mSounds[sound.name.hash].get(), group, paused, &channel);
        ERRCHECK(result);

        return channel;
    }

    FMOD::ChannelGroup *
    AudioManager::_getChannelGroup(ChannelGroup cg) {
        assert(mChannelGroup[cg]);
        auto *group = mChannelGroup[cg].get();
        assert(group);
        return group;
    }

    void
    AudioManager::playOneShot(const Sound &sound,
                              const SoundPitch &pitch,
                              const vec3 &distance) {
        auto *channel = _createChannel(sound);
        assert(channel);

        auto result = channel->setMode(FMOD_3D);
        ERRCHECK(result);

        _setPitch(*channel, pitch.value);

        // TODO: Does this matter?
        vec3 velocity{1.f, 1.f, 0.f};
        _set3DPos(*channel, distance, velocity);

        _unPause(*channel);
    }

    static FMOD_RESULT
    endOfPlayCallback(FMOD_CHANNELCONTROL *channelControl,
                      FMOD_CHANNELCONTROL_TYPE type,
                      FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType,
                      void *,
                      void *) {
        if (type == FMOD_CHANNELCONTROL_TYPE::FMOD_CHANNELCONTROL_CHANNELGROUP) {
            return FMOD_OK;
        }

        auto *channel = (FMOD::Channel *) (channelControl);
        if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_TYPE::FMOD_CHANNELCONTROL_CALLBACK_END) {
            void *finished;
            auto result = channel->getUserData(&finished);
            ERRCHECK(result);
            *(bool *) finished = true;
        }

        return FMOD_OK;
    }

    AudioManager::EntitySoundTag
    AudioManager::_createEntitySoundID(SoundName,
                                       EntityID id) {
        // TODO: FIX THIS
        assert(false);
        //auto result = pack_u32(enumCast(tag), id);
        return id;
    }

    AudioManager::EntityChannel &
    AudioManager::_getOrCreateLooping3DChannel(const Sound &sound,
                                               EntityID entityID) {
        auto id = _createEntitySoundID(sound.name, entityID);
        if (mLoopingChannels.find(id) == mLoopingChannels.end()) {
            EntityChannel entityChannel;
            entityChannel.entityID = entityID;
            entityChannel.channel = _createChannel(sound);
            entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);

            mLoopingChannels[id] = entityChannel;
        }
        return mLoopingChannels[id];
    }

    void
    AudioManager::_setPitch(FMOD::Channel &channel,
                            r32 pitch) {
        if (pitch > 256) {
            pitch = 256;
        }
        auto result = channel.setPitch(pitch);
        ERRCHECK(result);
    }

    bool
    AudioManager::_isPaused(FMOD::Channel &channel) {
        bool paused{false};
        auto result = channel.getPaused(&paused);
        ERRCHECK(result);
        return paused;
    }

    void
    AudioManager::_unPause(FMOD::Channel &channel) {
        auto result = channel.setPaused(false);
        ERRCHECK(result);
    }

    void
    AudioManager::_pause(FMOD::Channel &channel) {
        auto result = channel.setPaused(true);
        ERRCHECK(result);
    }

    void
    AudioManager::FMODDeleter::operator()(FMOD::Sound *s) const {
        s->release();
    }

    void
    AudioManager::FMODDeleter::operator()(FMOD::System *sys) const {
        sys->close();
        sys->release();
    }

    void
    AudioManager::FMODDeleter::operator()(FMOD::ChannelGroup *channel) const {
        channel->stop();
        channel->release();
    }

    void
    AudioManager::_set3DPos(FMOD::Channel &channel,
                            const vec3 &pos,
                            const vec3 &velocity) {
        FMOD_VECTOR fPos;
        fPos.x = pos.x;
        fPos.y = pos.y;
        fPos.z = pos.z;
        FMOD_VECTOR fVelocity;
        fVelocity.x = velocity.x;
        fVelocity.y = velocity.y;
        fVelocity.z = velocity.z;

        auto result = channel.set3DAttributes(&fPos, &fVelocity);
        ERRCHECK(result);

        result = channel.set3DMinMaxDistance(20.f, mMaxAudibleDistance); // In meters
        ERRCHECK(result);
    }

    void
    AudioManager::setChannelGroupVolume(ChannelGroup channelGroup,
                                        r32 volume) {
        auto effectiveVolume = volume * mMasterVolume;
        auto *group = _getChannelGroup(channelGroup);
        auto result = group->setVolume(effectiveVolume);
        ERRCHECK(result);
        mChannelVolume[channelGroup] = volume;
    }

    void
    AudioManager::setMasterVolume(r32 volume) {
        assert(almost_Positive(volume) || almost_Zero(volume));
        assert(almost_Less(volume, 1.f));
        mMasterVolume = volume;

        for (auto i = enumCast(ChannelGroup::UNKNOWN) + 1;
             i < enumCast(ChannelGroup::LAST); ++i) {
            auto channelGroup = static_cast<ChannelGroup>(i);
            setChannelGroupVolume(channelGroup, volume);
        }
    }
}
