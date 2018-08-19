#pragma once

#include <map>
#include <mutex>

#include <oni-core/io/input.h>

namespace oni {
    namespace entities {
        typedef std::map<common::PeerID, common::EntityID> ClientCarEntityMap;
        typedef std::map<common::PeerID, io::Input> ClientInputMap;

        class ClientDataManager {
        public:
            ClientDataManager();

            ~ClientDataManager();

            void addNewClient(common::PeerID clientID, common::EntityID entityID);

            void deleteClient(common::PeerID clientID);

            void setClientInput(common::PeerID clientID, const io::Input &input);

            const io::Input& getClientInput(common::PeerID clientID);

            common::EntityID getEntityID(common::PeerID clientID) const;

            std::unique_lock<std::mutex> scopedLock();

            void lock();

            void unlock();

        private:
            ClientCarEntityMap mClientCarEntityMap{};
            ClientInputMap mClientInputMap{};

            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };
    }
}
