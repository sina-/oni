#pragma once

#include <map>
#include <mutex>

#include <oni-core/io/input.h>

namespace oni {
    namespace entities {
        typedef std::map<common::PeerID, common::EntityID> ClientToCarEntity;
        typedef std::vector<common::PeerID> ClientList;
        typedef std::map<common::PeerID, io::Input> ClientToInput;
        typedef std::vector<io::Input> ClientInputList;
        typedef std::vector<common::EntityID> CarEntities;

        class ClientDataManager {
        public:
            ClientDataManager();

            ~ClientDataManager();

            void addNewClient(const common::PeerID &clientID, common::EntityID entityID);

            void deleteClient(const common::PeerID &clientID);

            void setClientInput(const common::PeerID &clientID, const io::Input &input);

            const io::Input &getClientInput(const common::PeerID &clientID);

            ClientInputList getClientsInput() const;

            CarEntities getCarEntities() const;

            const ClientList &getClients() const;

            size_t getNumClients() const;

            common::EntityID getEntityID(const common::PeerID &clientID) const;

            std::unique_lock<std::mutex> scopedLock();

/*            void lock();

            void unlock();*/

        private:
            ClientToCarEntity mClientToCarEntity{};
            ClientToInput mClientToInput{};
            ClientList mClients{};

            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };
    }
}
