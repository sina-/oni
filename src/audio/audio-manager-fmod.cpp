#include <oni-core/audio/audio-manager-fmod.h>

#include <cassert>

#include <fmod.hpp>

#include <oni-core/common/consts.h>
#include <oni-core/common/defines.h>
#include <oni-core/entities/entity-factory.h>
#include <oni-core/component/geometry.h>

#define ERRCHECK(_result) assert((_result) == FMOD_OK)
#define VALID(MAP, ID) assert(MAP.find(id) != MAP.end())

namespace oni {
    namespace audio {
        AudioManagerFMOD::AudioManagerFMOD() : AudioManager() {
            FMOD::System *system{nullptr};

            auto result = FMOD::System_Create(&system);
            ERRCHECK(result);
            mSystem = std::unique_ptr<FMOD::System, FMODDeleter>(system, FMODDeleter());

            common::uint32 version;
            result = system->getVersion(&version);
            ERRCHECK(result);
            assert(version >= FMOD_VERSION);

            result = system->init(32, FMOD_INIT_NORMAL, nullptr);
            ERRCHECK(result);

            result = system->update();
            ERRCHECK(result);
        }

        void AudioManagerFMOD::tick(entities::EntityFactory &entityFactory) {
            auto result = mSystem->update();
            ERRCHECK(result);

            auto view = entityFactory.getEntityManager().createViewWithLock<component::SoundID,
                    component::SoundPlaybackState,
                    component::Placement>();
            for (auto &&entity : view) {
                auto &playbackState = view.get<component::SoundPlaybackState>(entity);
                auto &soundID = view.get<component::SoundID>(entity);
                if (playbackState == component::SoundPlaybackState::PLAY) {
                    tryPlay(soundID, entity);
                } else if (playbackState == component::SoundPlaybackState::STOP) {
                    kill(soundID, entity);
                } else if (playbackState == component::SoundPlaybackState::FADE_OUT) {
                    fadeOut(soundID, entity);
                }
            }

            // TODO: This is not thread safe. Other threads could write to this data.
            for (auto sound = mChannels.begin(); sound != mChannels.end(); ++sound) {
                bool playing{false};
                result = sound->second->isPlaying(&playing);
                if (result == FMOD_ERR_INVALID_HANDLE) {
                    // TODO: Not super happy with this. I should be able to just have one-shot play type of audio
                    // that does not require manual clean-up. Why am I even storing this information for non-looping
                    // sounds?
                    mChannels.erase(sound);
                }
            }
        }

        void AudioManagerFMOD::loadSound(const component::SoundID &id) {
            FMOD::Sound *sound{};
            auto result = mSystem->createSound(id.c_str(), FMOD_DEFAULT, nullptr, &sound);
            ERRCHECK(result);
            assert(sound);

            mSounds[id] = std::unique_ptr<FMOD::Sound, FMODDeleter>(sound, FMODDeleter());
        }

        void AudioManagerFMOD::attachControls(const component::SoundID &id) {
            mChannels[id] = createChannel(id);
        }

        std::unique_ptr<FMOD::Channel, AudioManagerFMOD::FMODDeleter>
        AudioManagerFMOD::createChannel(const component::SoundID &id) {
            VALID(mSounds, id);

            FMOD::Channel *channel{nullptr};
            auto result = mSystem->playSound(mSounds[id].get(), nullptr, true, &channel);
            ERRCHECK(result);

            return std::unique_ptr<FMOD::Channel, FMODDeleter>(channel, FMODDeleter());
        }

        void AudioManagerFMOD::play(const component::SoundID &id) {
            if (mChannels.find(id) != mChannels.end()) {
                // NOTE: For non-looping sounds, after one play through the channel is not valid anymore.
                auto result = mChannels[id]->setPaused(false);
                ERRCHECK(result);
            } else {
                VALID(mSounds, id);
                auto result = mSystem->playSound(mSounds[id].get(), nullptr, false, nullptr);
                ERRCHECK(result);
            }
        }

        static FMOD_RESULT
        endOfPlayCallback(FMOD_CHANNELCONTROL *channelControl, FMOD_CHANNELCONTROL_TYPE type,
                          FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, void *, void *) {
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
        };

        void AudioManagerFMOD::tryPlay(const component::SoundID &soundID, common::EntityID entityID) {
            auto soundEntityID = getID(soundID, entityID);
            if (mEntityChannel.find(soundEntityID) == mEntityChannel.end()) {
                EntityChannel entityChannel;
                entityChannel.entityID = entityID;
                entityChannel.channel = createChannel(soundID);
                // TODO: This is
                auto result = entityChannel.channel->setVolume(0.5f);
                ERRCHECK(result);

                mEntityChannel[soundEntityID] = std::move(entityChannel);
            }

            auto &entityChannel = mEntityChannel.at(soundEntityID);
            bool paused;
            auto result = entityChannel.channel->getPaused(&paused);
            // NOTE: This will happen if the play-back has ended.
            if (result == FMOD_ERR_INVALID_HANDLE) {
                mEntityChannel.erase(soundEntityID);
                tryPlay(soundID, entityID);
            } else if (paused) {
                result = entityChannel.channel->setPaused(false);
                ERRCHECK(result);
            } else {
                // NOTE: The sound is already playing.
            }
        }

        void AudioManagerFMOD::setLoop(const component::SoundID &id, bool loop) {
            VALID(mChannels, id);

            if (loop) {
                mChannels[id]->setMode(FMOD_LOOP_NORMAL);
            } else {
                mChannels[id]->setMode(FMOD_LOOP_OFF);
            }
        }

        void AudioManagerFMOD::setPitch(const component::SoundID &id, common::real32 pitch) {
            if (pitch > 256) {
                pitch = 256;
            }
            VALID(mChannels, id);
            auto result = mChannels[id]->setPitch(pitch);
            ERRCHECK(result);
        }

        common::real64 AudioManagerFMOD::pauseSound(const component::SoundID &id) {
            // NOTE: It might be that the play-back has already finished.
            if (mChannels.find(id) == mChannels.end()) {
                return -1.f;
            }

            auto result = mChannels[id]->setPaused(true);
            ERRCHECK(result);

            common::uint32 pos;
            result = mChannels[id]->getPosition(&pos, FMOD_TIMEUNIT_MS);
            ERRCHECK(result);

            return (static_cast<common::real64>(pos) + common::EP);
        }

        void AudioManagerFMOD::kill(const component::SoundID &soundID, common::EntityID entityID) {
            auto soundEntityID = getID(soundID, entityID);
            if (mEntityChannel.find(soundEntityID) == mEntityChannel.end()) {
                assert(false);
                return;
            }

            auto &entityChannel = mEntityChannel.at(soundEntityID);
            auto result = entityChannel.channel->stop();
            ERRCHECK(result);

            mEntityChannel.erase(soundEntityID);
        }

        void AudioManagerFMOD::setVolume(const component::SoundID &id, common::real32 volume) {
            VALID(mChannels, id);
            auto result = mChannels[id]->setVolume(volume);
            ERRCHECK(result);
        }

        bool AudioManagerFMOD::isPlaying(const component::SoundID &id) {
            VALID(mChannels, id);
            bool isPaused{false};
            auto result = mChannels[id]->getPaused(&isPaused);
            ERRCHECK(result);
            return !isPaused;
        }

        void AudioManagerFMOD::seek(const component::SoundID &id, common::real64 position) {
            VALID(mChannels, id);
            auto result = mChannels[id]->setPosition(static_cast<common::uint32>(position + common::EP),
                                                     FMOD_TIMEUNIT_MS);
            ERRCHECK(result);
        }

        void AudioManagerFMOD::fadeOut(const component::SoundID &id) {
            //VALID(mChannels, id);
        }

        AudioManagerFMOD::SoundEntityID AudioManagerFMOD::getID(const component::SoundID &soundID,
                                                                common::EntityID entityID) {
            return soundID + std::to_string(entityID);
        }

        void AudioManagerFMOD::fadeOut(const component::SoundID &soundID, common::EntityID entityID) {
            // TODO: Proper fade-out
            kill(soundID, entityID);
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::Sound *s) const {
            s->release();
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::System *sys) const {
            sys->close();
            sys->release();
        }

        void AudioManagerFMOD::FMODDeleter::operator()(FMOD::Channel *channel) const {
            UNUSED(channel);
        }
    }
}