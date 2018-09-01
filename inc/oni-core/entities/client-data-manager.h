#pragma once

#include <map>
#include <mutex>

#include <oni-core/io/input.h>

namespace oni {
    namespace entities {
        typedef std::map<common::PeerID, common::EntityID> ClientCarEntityMap;
        typedef std::vector<common::PeerID> Clients;
        typedef std::map<common::PeerID, io::Input> ClientInputMap;
        typedef std::vector<io::Input> ClientsInput;
        typedef std::vector<common::EntityID> ClientsCarEntity;

        class ClientDataManager {
        public:
            ClientDataManager();

            ~ClientDataManager();

            void addNewClient(const common::PeerID &clientID, common::EntityID entityID);

            void deleteClient(const common::PeerID &clientID);

            void setClientInput(const common::PeerID &clientID, const io::Input &input);

            const io::Input &getClientInput(const common::PeerID &clientID);

            ClientsInput getClientsInput() const;

            ClientsCarEntity getClientsCarEntity() const;

            const Clients &getClients() const;

            size_t numClients() const;

            common::EntityID getEntityID(const common::PeerID &clientID) const;

            std::unique_lock<std::mutex> scopedLock();

/*            void lock();

            void unlock();*/

        private:
            ClientCarEntityMap mClientCarEntityMap{};
            ClientInputMap mClientInputMap{};
            Clients mClients{};

            std::mutex mMutex{};
            std::unique_lock<std::mutex> mLock{};
        };
    }
}
