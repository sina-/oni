#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/fwd.h>


namespace oni {
    class EntityFactory {
    public:
        explicit EntityFactory(const std::string &);

        EntityID
        createEntity(EntityManager &,
                     EntityName);

    private:


    };
}