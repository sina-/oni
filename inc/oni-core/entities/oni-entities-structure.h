#pragma once

#include<oni-core/util/oni-util-hash.h>


namespace oni {
    struct Component_Name : public HashedString {
    };

    struct EntityType_Name : public HashedString {
    };

    struct EntityName : public HashedString {
    };
}

DEFINE_STD_HASH_FUNCTIONS(oni::Component_Name)
DEFINE_STD_HASH_FUNCTIONS(oni::EntityType_Name)
DEFINE_STD_HASH_FUNCTIONS(oni::EntityName)
