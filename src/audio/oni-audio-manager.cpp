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

        loadChannels();
        preLoadSounds();
    }

    void
    AudioManager::tick(const WorldP3D &playerPos) {
        mPlayerPos = playerPos;
        auto result = mSystem->update();
        ERRCHECK(result);
    }

    void
    AudioManager::playCollisionSoundEffect(const Event_Collision &event) {
        static_assert(sizeof(event.pcPair.a) == sizeof(u8), "Hashing will fail due to size mismatch");
        auto collisionTag = createCollisionEffectID(event.pcPair);
        auto distance = mPlayerPos.value - event.pos.value;
        auto soundTag = mCollisionEffects[collisionTag];
        assert(soundTag);
        auto sound = Sound{soundTag, ChannelGroup::EFFECT};
        auto pitch = SoundPitch{1.f};
        playOneShot(sound, pitch, distance);
    }

    AudioManager::CollisionSoundTag
    AudioManager::createCollisionEffectID(const PhysicalCatPair &pcp) {
        auto a = enumCast(pcp.a);
        auto b = enumCast(pcp.b);

        if (a > b) {
            std::swap(a, b); // Assuming soundEffect for A->B collision is same as B->A
        }

        auto soundID = pack_u8(a, b);
        return soundID;
    }

    void
    AudioManager::loadChannels() {
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
    AudioManager::preLoadSounds() {
        // TODO: The sound must have been loaded using the asset packet instead of just going through all the
        // sound types and shotgun loading all
        for (auto i = enumCast(Sound_Tag::UNKNOWN) + 1;
             i < enumCast(Sound_Tag::LAST);
             ++i) {
            auto tag = static_cast<Sound_Tag>(i);
            auto path = mAssetManager.getAssetFilePath(tag);
            loadSound(tag, path);
        }

        for (auto i = enumCast(PhysicalCategory::UNKNOWN);
             i < enumCast(PhysicalCategory::LAST);
             ++i) {
            auto id = createCollisionEffectID({PhysicalCategory::ROCKET,
                                               static_cast<PhysicalCategory >(i)});
            mCollisionEffects[id] = Sound_Tag::COLLISION_ROCKET_UNKNOWN;
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
    AudioManager::loadSound(Sound_Tag tag,
                            std::string_view filePath) {
//            if (mSounds[tag]) {
//                return;
//            }

        FMOD::Sound *sound{};
        auto result = mSystem->createSound(filePath.data(), FMOD_DEFAULT, nullptr, &sound);
        ERRCHECK(result);
        assert(sound);

        std::unique_ptr<FMOD::Sound, FMODDeleter> value(sound);
        mSounds.insert({tag, std::move(value)});
    }

    FMOD::Channel *
    AudioManager::createChannel(const Sound &sound) {
        assert(mChannelGroup[sound.group]);
        auto *group = mChannelGroup[sound.group].get();
        assert(group);
        assert(mSounds[sound.tag]);

        FMOD::Channel *channel{nullptr};
        auto paused = true;
        auto result = mSystem->playSound(mSounds[sound.tag].get(), group, paused, &channel);
        ERRCHECK(result);

        return channel;
    }

    FMOD::ChannelGroup *
    AudioManager::getChannelGroup(ChannelGroup channelGroup) {
        assert(mChannelGroup[channelGroup]);
        auto *group = mChannelGroup[channelGroup].get();
        assert(group);
        return group;
    }

    void
    AudioManager::playOneShot(const Sound &sound,
                              const SoundPitch &pitch,
                              const vec3 &distance) {
        auto *channel = createChannel(sound);
        assert(channel);

        auto result = channel->setMode(FMOD_3D);
        ERRCHECK(result);

        setPitch(*channel, pitch.value);

        // TODO: Does this matter?
        vec3 velocity{1.f, 1.f, 0.f};
        set3DPos(*channel, distance, velocity);

        unPause(*channel);
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
    AudioManager::createEntitySoundID(Sound_Tag tag,
                                      EntityID id) {
        auto result = pack_u32(enumCast(tag), id);
        return result;
    }

    AudioManager::EntityChannel &
    AudioManager::getOrCreateLooping3DChannel(const Sound &sound,
                                              EntityID entityID) {
        auto id = createEntitySoundID(sound.tag, entityID);
        if (mLoopingChannels.find(id) == mLoopingChannels.end()) {
            EntityChannel entityChannel;
            entityChannel.entityID = entityID;
            entityChannel.channel = createChannel(sound);
            entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);

            mLoopingChannels[id] = entityChannel;
        }
        return mLoopingChannels[id];
    }

    void
    AudioManager::setPitch(FMOD::Channel &channel,
                           r32 pitch) {
        if (pitch > 256) {
            pitch = 256;
        }
        auto result = channel.setPitch(pitch);
        ERRCHECK(result);
    }

    bool
    AudioManager::isPaused(FMOD::Channel &channel) {
        bool paused{false};
        auto result = channel.getPaused(&paused);
        ERRCHECK(result);
        return paused;
    }

    void
    AudioManager::unPause(FMOD::Channel &channel) {
        auto result = channel.setPaused(false);
        ERRCHECK(result);
    }

    void
    AudioManager::pause(FMOD::Channel &channel) {
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
    AudioManager::set3DPos(FMOD::Channel &channel,
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

    r32
    AudioManager::getChannelGroupVolume(ChannelGroup channelGroup) {
        auto result = mChannelVolume[channelGroup];
        return result;
    }

    void
    AudioManager::setChannelGroupVolume(ChannelGroup channelGroup,
                                        r32 volume) {
        auto effectiveVolume = volume * mMasterVolume;
        auto *group = getChannelGroup(channelGroup);
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
