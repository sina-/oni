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
        // TODO: Do I need this?
        mPlayerPos = playerPos;
        auto result = mSystem->update();
        ERRCHECK(result);
    }

    void
    AudioManager::_preLoadSounds() {
        for (auto iter = mAssetManager.soundAssetsBegin(); iter != mAssetManager.soundAssetsEnd(); ++iter) {
            const auto &soundAsset = iter->second;
            _loadSound(soundAsset);
        }
    }

    void
    AudioManager::_loadChannels() {
        FMOD::ChannelGroup *group{nullptr};
        auto result = mSystem->createChannelGroup("effectsChannel", &group);
        ERRCHECK(result);
        auto effectsGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
        effectsGroup->setVolume(1.f);
        mChannelGroup[ChannelGroup::GET("effect").id] = std::move(effectsGroup);
        group = nullptr;

        result = mSystem->createChannelGroup("musicChannel", &group);
        ERRCHECK(result);
        auto musicGroup = std::unique_ptr<FMOD::ChannelGroup, FMODDeleter>(group, FMODDeleter());
        musicGroup->setVolume(1.f);
        mChannelGroup[ChannelGroup::GET("music").id] = std::move(musicGroup);
        group = nullptr;

        for (auto channelGroup = ChannelGroup::begin() + 1; channelGroup != ChannelGroup::end(); ++channelGroup) {
            setChannelGroupVolume(*channelGroup, 1.f);
        }
    }

    void
    AudioManager::kill(EntityID entityID) {
        for (auto it = mLoopingChannels.begin(); it != mLoopingChannels.end();) {
            if (it->entityID == entityID) {
                auto result = it->channel->stop();
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
        if (sound.group.id < 0 || sound.group.id > ChannelGroup::size()) {
            assert(false);
            return nullptr;
        }
        auto *group = mChannelGroup[sound.group.id].get();

        auto soundToPlay = mSounds.find(sound.name.hash);
        if (soundToPlay == mSounds.end()) {
            assert(false);
            return nullptr;
        }

        FMOD::Channel *channel{};
        auto paused = true;
        auto result = mSystem->playSound(soundToPlay->second.get(), group, paused, &channel);

        ERRCHECK(result);

        return channel;
    }

    FMOD::ChannelGroup *
    AudioManager::_getChannelGroup(ChannelGroup cg) {
        if (cg.id < 0 || cg.id > ChannelGroup::size()) {
            assert(false);
            return nullptr;
        }
        auto *group = mChannelGroup[cg.id].get();
        assert(group);
        return group;
    }

    void
    AudioManager::playOneShot(const Sound &sound,
                              const SoundPitch &pitch,
                              const vec3 &distance) {
        auto *channel = _createChannel(sound);
        if (!channel) {
            assert(false);
            return;
        }

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

    AudioManager::EntityChannel *
    AudioManager::_getOrCreateLooping3DChannel(const Sound &sound,
                                               EntityID entityID) {
        if (!sound.name.valid()) {
            assert(false);
            return nullptr;
        }

        if (entityID == EntityManager::nullEntity()) {
            assert(false);
            return nullptr;
        }

        for (auto it = mLoopingChannels.begin(); it != mLoopingChannels.end();) {
            if (it->entityID == entityID && it->name == sound.name) {
                return &(*it);
            } else {
                ++it;
            }
        }

        EntityChannel entityChannel;
        entityChannel.entityID = entityID;
        entityChannel.name = sound.name;
        entityChannel.channel = _createChannel(sound);
        entityChannel.channel->setMode(FMOD_LOOP_NORMAL | FMOD_3D);
        mLoopingChannels.push_back(entityChannel);
        return &mLoopingChannels.back();
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
    AudioManager::_updateChannelVolume(ChannelGroup cg) {
        auto channelVolume = mChannelVolume[cg.id];
        setChannelGroupVolume(cg, channelVolume);
    }

    void
    AudioManager::setChannelGroupVolume(ChannelGroup channelGroup,
                                        r32 volume) {
        auto effectiveVolume = volume * mMasterVolume;
        auto *group = _getChannelGroup(channelGroup);
        auto result = group->setVolume(effectiveVolume);
        ERRCHECK(result);
        mChannelVolume[channelGroup.id] = volume;
    }

    void
    AudioManager::setMasterVolume(r32 volume) {
        assert(almost_Positive(volume) || almost_Zero(volume));
        assert(almost_Less(volume, 1.f));
        mMasterVolume = volume;

        for (auto iter = ChannelGroup::begin() + 1;
             iter < ChannelGroup::end(); ++iter) {
            _updateChannelVolume(*iter);
        }
    }
}
