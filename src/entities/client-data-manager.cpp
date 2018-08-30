#include <oni-core/entities/client-data-manager.h>

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
        }

        void ClientDataManager::deleteClient(const common::PeerID &clientID) {
            mClientCarEntityMap.erase(clientID);
        }

        void ClientDataManager::setClientInput(const common::PeerID &clientID, const io::Input &input) {
            mClientInputMap[clientID] = input;
        }

        const io::Input &ClientDataManager::getClientInput(const common::PeerID &clientID) {
            return mClientInputMap[clientID];
        }

    }
}