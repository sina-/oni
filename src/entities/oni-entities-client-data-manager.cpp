#include <oni-core/entities/oni-entities-client-data-manager.h>

#include <algorithm>
#include <cassert>

#include <oni-core/common/oni-common-typedefs-network.h>


namespace oni {
    ClientDataManager::ClientDataManager() {
        mLock = std::unique_lock<std::mutex>(mMutex, std::defer_lock);
    }

    ClientDataManager::~ClientDataManager() = default;

    std::unique_lock<std::mutex>
    ClientDataManager::scopedLock() {
        return std::unique_lock<std::mutex>(mMutex);
    }

/*        void ClientDataManager::lock() {
            mLock.lock();
        }

        void ClientDataManager::unlock() {
            mLock.unlock();
        }*/

    EntityID
    ClientDataManager::getEntityID(const std::string &clientID) const {
        assert(mClientToCarEntity.find(clientID) != mClientToCarEntity.end());
        return mClientToCarEntity.at(clientID);
    }

    void
    ClientDataManager::addNewClient(const std::string &clientID,
                                    EntityID entityID) {
        mClientToCarEntity[clientID] = entityID;
        mCarEntityToClient[entityID] = clientID;
        mCarEntityToInput[entityID] = Input{};
        mClients.push_back(clientID);
    }

    void
    ClientDataManager::deleteClient(const std::string &clientID) {
        auto carID = mClientToCarEntity[clientID];
        if (carID) {
            mCarEntityToInput.erase(carID);
            mClientToCarEntity.erase(clientID);
        }

        if (auto it = std::find_if(mClients.begin(), mClients.end(),
                                   [&](const std::string &peerID) { return (peerID == clientID); });
                it != mClients.end()) {
            mClients.erase(it);
        } else {
            assert(false);
        }
    }

    void
    ClientDataManager::setClientInput(const std::string &clientID,
                                      const Input &input) {
        auto carID = mClientToCarEntity[clientID];
        if (carID) {
            mCarEntityToInput[carID] = input;
        }
    }

    CarEntities
    ClientDataManager::getCarEntities() const {
        CarEntities entities{};
        for (const auto &entity : mClientToCarEntity) {
            entities.push_back(entity.second);
        }
        return entities;
    }

    size_t
    ClientDataManager::getNumClients() const {
        return mClients.size();
    }

    void
    ClientDataManager::resetClientsInput() {
        for (auto &&input:mCarEntityToInput) {
            input.second.reset();
        }
    }

    const Input *
    ClientDataManager::getClientInput(const EntityID &entityID) const {
        auto input = mCarEntityToInput.find(entityID);
        if (input == mCarEntityToInput.end()) {
            return nullptr;
        }
        return &mCarEntityToInput.at(entityID);
    }
}