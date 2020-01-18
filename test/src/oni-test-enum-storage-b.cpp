#include <oni-core/common/oni-common-typedef.h>
#include <oni-test/oni-test-enum-storage.h>

namespace oni {
    const oni::c8 *
getEnumStorageAddressTU_B() {
        return Enum_Storage::GET("second").name.str.data();
    }
}
