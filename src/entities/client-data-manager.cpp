#include <oni-core/entities/client-data-manager.h>

#include <algorithm>
#include <assert.h>
#include <iostream>

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
            assert(mClientToCarEntity.find(clientID) != mClientToCarEntity.end());
            return mClientToCarEntity.at(clientID);
        }

        void ClientDataManager::addNewClient(const common::PeerID &clientID, common::EntityID entityID) {
            mClientToCarEntity[clientID] = entityID;
            mCarEntityToClient[entityID] = clientID;
            mCarEntityToInput[entityID] = io::Input{};
            mClients.push_back(clientID);
        }

        void ClientDataManager::deleteClient(const common::PeerID &clientID) {
            mCarEntityToInput.erase(mClientToCarEntity.at(clientID));
            mClientToCarEntity.erase(clientID);

            if (auto it = std::find_if(mClients.begin(), mClients.end(),
                                       [&](const common::PeerID &peerID) { return (peerID == clientID); });
                    it != mClients.end()) {
                mClients.erase(it);
            } else {
                assert(false);
            }
        }

        void ClientDataManager::setClientInput(const common::PeerID &clientID, const io::Input &input) {
            mCarEntityToInput[mClientToCarEntity.at(clientID)] = input;
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

        void ClientDataManager::resetClientsInput() {
            for (auto &&input:mCarEntityToInput) {
                input.second.reset();
            }
        }

        const io::Input *ClientDataManager::getClientInput(const common::EntityID &entityID) const {
            auto input = mCarEntityToInput.find(entityID);
            if(input == mCarEntityToInput.end()){
                return nullptr;
            }
            return &mCarEntityToInput.at(entityID);
        }

        common::PeerID ClientDataManager::getPeerID(const common::EntityID &entityID) const {
            assert(mCarEntityToClient.find(entityID) != mCarEntityToClient.end());
            return mCarEntityToClient.at(entityID);
        }
    }
}