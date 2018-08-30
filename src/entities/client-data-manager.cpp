#include <oni-core/entities/client-data-manager.h>

#include <algorithm>
#include <assert.h>

namespace oni {
    namespace entities {

        ClientDataManager::ClientDataManager() {
            mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
        }

        ClientDataManager::~ClientDataManager() = default;

        std::unique_lock<std::mutex> ClientDataManager::scopedLock() {
            return std::unique_lock<std::mutex>(mMutex);
        }

/*        void ClientDataManager::lock() {
            mLock.lock();
        }

        void ClientDataManager::unlock() {
            mLock.unlock();
        }*/

        common::EntityID ClientDataManager::getEntityID(const common::PeerID &clientID) const {
            return mClientCarEntityMap.at(clientID);
        }

        void ClientDataManager::addNewClient(const common::PeerID &clientID, common::EntityID entityID) {
            mClientCarEntityMap[clientID] = entityID;
            mClients.push_back(clientID);
        }

        void ClientDataManager::deleteClient(const common::PeerID &clientID) {
            mClientCarEntityMap.erase(clientID);
            mClientInputMap.erase(clientID);

            auto it = std::find_if(mClients.begin(), mClients.end(),
                                   [&](const common::PeerID &peerID) { return (peerID == clientID); });
            if (it != mClients.end()) {
                mClients.erase(it);
            } else {
                assert(false);
            }
        }

        void ClientDataManager::setClientInput(const common::PeerID &clientID, const io::Input &input) {
            mClientInputMap[clientID] = input;
        }

        const io::Input &ClientDataManager::getClientInput(const common::PeerID &clientID) {
            return mClientInputMap[clientID];
        }

        const Clients &ClientDataManager::getClients() const {
            return mClients;
        }

        ClientsInput ClientDataManager::getClientsInput() const {
            ClientsInput inputs{};
            for (const auto &input : mClientInputMap) {
                inputs.push_back(input.second);
            }
            return inputs;
        }

        ClientsCarEntity ClientDataManager::getClientsCarEntity() const {
            ClientsCarEntity entities{};
            for (const auto &entity : mClientCarEntityMap) {
                entities.push_back(entity.second);
            }
            return entities;
        }

    }
}