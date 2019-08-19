#pragma once

#include <map>
#include <mutex>

#include <oni-core/io/oni-io-input.h>


namespace oni {
    typedef std::map<std::string, EntityID> ClientToCarEntity;
    typedef std::map<EntityID, std::string> CarEntityToClient;
    typedef std::map<EntityID, Input> CarEntityToInput;
    typedef std::vector<std::string> ClientList;
    typedef std::vector<EntityID> CarEntities;

    class ClientDataManager {
    public:
        ClientDataManager();

        ~ClientDataManager();

        void
        addNewClient(const std::string &clientID,
                     EntityID entityID);

        void
        deleteClient(const std::string &clientID);

        void
        setClientInput(const std::string &clientID,
                       const Input &input);

        const Input *
        getClientInput(const EntityID &entityID) const;

        void
        resetClientsInput();

        CarEntities
        getCarEntities() const;

        size_t
        getNumClients() const;

        EntityID
        getEntityID(const std::string &clientID) const;

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
