#pragma once

#include <map>
#include <mutex>

#include <oni-core/io/oni-io-input.h>

namespace oni {
    namespace entities {
        typedef std::map<common::PeerID, common::EntityID> ClientToCarEntity;
        typedef std::map<common::EntityID, common::PeerID> CarEntityToClient;
        typedef std::map<common::EntityID, io::Input> CarEntityToInput;
        typedef std::vector<common::PeerID> ClientList;
        typedef std::vector<common::EntityID> CarEntities;

        class ClientDataManager {
        public:
            ClientDataManager();

            ~ClientDataManager();

            void
            addNewClient(const common::PeerID &clientID,
                         common::EntityID entityID);

            void
            deleteClient(const common::PeerID &clientID);

            void
            setClientInput(const common::PeerID &clientID,
                           const io::Input &input);

            const io::Input *
            getClientInput(const common::EntityID &entityID) const;

            void
            resetClientsInput();

            CarEntities
            getCarEntities() const;

            size_t
            getNumClients() const;

            common::EntityID
            getEntityID(const common::PeerID &clientID) const;

            std::unique_lock<std::mutex>
            scopedLock();

/*            void lock();

            void unlock();*/

        private:
            ClientToCarEntity mClientToCarEntity{};
            CarEntityToClient mCarEntityToClient{};
            CarEntityToInput mCarEntityToInput{};
            ClientList mClients{};

            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };
    }
}
