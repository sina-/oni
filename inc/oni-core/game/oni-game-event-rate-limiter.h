#pragma once

#include <vector>
#include <unordered_map>

#include <oni-core/game/oni-game-event.h>

namespace oni {
    class EventRateLimiter {
    public:
        using EventID = oni::size;

        EventRateLimiter() {
            auto countType = enumCast(EventDispatcherType::LAST);
            mRateLimits.resize(countType);
        }

        template<class Event>
        void
        registerEventCD(EventDispatcherType type,
                        const CoolDown &cd) {
            auto typeID = enumCast(type);
            assert(typeID < enumCast(EventDispatcherType::LAST));
            auto &cds = mRateLimits[enumCast(type)];
            auto eventID = _getEventID<Event>(type);
            auto iter = cds.find(eventID);
            if (iter == cds.end()) {
                cds.emplace(eventID, cd);
            } else {
                assert(false);
            }
        }

        template<class Event>
        bool
        canFire(EventDispatcherType type) {
            auto eventID = _getEventID<Event>(type);
            auto *cd = _getCD(type, eventID);
            if (!cd) {
                return true;
            }

            // TODO: This needs to happen somewhere else?
            auto dt = 0.1;
            subAndZeroClip(cd->current, dt);

            if (cd->current > 0) {
                return false;
            }
            cd->current = cd->initial;
            return true;
        };

    private:
        CoolDown *
        _getCD(EventDispatcherType type,
               EventID id) {
            auto typeID = enumCast(type);
            assert(typeID < enumCast(EventDispatcherType::LAST));
            auto &cds = mRateLimits[typeID];
            auto cd = cds.find(id);
            if (cd != cds.end()) {
                return &(cd->second);
            }
            return nullptr;
        }

        template<class Event>
        EventID
        _getEventID(EventDispatcherType type) {
            auto typeID = enumCast(type);
            assert(typeID < enumCast(EventDispatcherType::LAST));

            static const auto event = Event{};
            return reinterpret_cast<EventID> (&event);
        }

    private:
        std::vector<std::unordered_map<EventID, CoolDown>> mRateLimits;
    };
}
