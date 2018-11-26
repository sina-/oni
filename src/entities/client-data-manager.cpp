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
            return mClientToCarEntity.at(clientID);
        }

        void ClientDataManager::addNewClient(const common::PeerID &clientID, common::EntityID entityID) {
            mClientToCarEntity[clientID] = entityID;
            mClients.push_back(clientID);
        }

        void ClientDataManager::deleteClient(const common::PeerID &clientID) {
            mClientToCarEntity.erase(clientID);
            mClientToInput.erase(clientID);

            auto it = std::find_if(mClients.begin(), mClients.end(),
                                   [&](const common::PeerID &peerID) { return (peerID == clientID); });
            if (it != mClients.end()) {
                mClients.erase(it);
            } else {
                assert(false);
            }
        }

        void ClientDataManager::setClientInput(const common::PeerID &clientID, const io::Input &input) {
            mClientToInput[clientID] = input;
        }

        const io::Input &ClientDataManager::getClientInput(const common::PeerID &clientID) {
            return mClientToInput[clientID];
        }

        const ClientList &ClientDataManager::getClients() const {
            return mClients;
        }

        ClientInputList ClientDataManager::getClientsInput() const {
            ClientInputList inputs{};
            for (const auto &input : mClientToInput) {
                inputs.push_back(input.second);
            }
            return inputs;
        }

        CarEntities ClientDataManager::getCarEntities() const {
            CarEntities entities{};
            for (const auto &entity : mClientToCarEntity) {
                entities.push_back(entity.second);
            }
            return entities;
        }

        size_t ClientDataManager::getNumClients() const {
            return mClients.size();
        }

    }
}