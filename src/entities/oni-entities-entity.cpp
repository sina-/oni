#include<oni-core/entities/oni-entities-entity.h>

#include<oni-core/math/oni-math-function.h>

namespace oni {
    bool
    operator==(const EntityPair &lhs,
               const EntityPair &rhs) {
        if (lhs.a == rhs.a && lhs.b == rhs.b) {
            return true;
        }
        if (lhs.a == rhs.b && lhs.b == rhs.a) {
            return true;
        }
        return false;
    }

    u64
    EntityPairHasher::operator()(const EntityPair &pair) const noexcept {
        if (pair.a < pair.b) {
            return pack_u32(pair.a, pair.b);
        }
        return pack_u32(pair.b, pair.a);
    }
}