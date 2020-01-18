#include <oni-core/common/oni-common-typedef.h>
#include <oni-test/oni-test-enum-storage.h>

namespace oni {
    const oni::c8 *
    getEnumStorageAddressTU_C() {
        return Enum_Storage::GET("third").name.str.data();
    }
}
