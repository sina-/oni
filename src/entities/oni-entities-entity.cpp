#include<oni-core/entities/oni-entities-entity.h>

#include<oni-core/math/oni-math-function.h>

namespace oni {
    u64
    EntityPairHasher::operator()(const EntityPair &pair) const noexcept {
        static auto stdHash = std::hash<u64>();
        if (pair.a < pair.b) {
            return stdHash(pack_u32(pair.a, pair.b));
        }
        return stdHash(pack_u32(pair.b, pair.a));
    }

    bool
    EntityPairEqual::operator()(const EntityPair &rhs,
                                const EntityPair &lhs) const noexcept {
        auto equalA = rhs.a == lhs.a && rhs.b == lhs.b;
        auto equalB = rhs.a == lhs.b && rhs.b == lhs.a;
        return equalA || equalB;
    }
}