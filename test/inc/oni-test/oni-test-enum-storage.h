#include <oni-core/util/oni-util-enum.h>

namespace oni {
    ONI_ENUM_DEF(Enum_Storage, { 1, "first" }, { 2, "second" }, { 3, "third" })

    const oni::c8 *
    getEnumStorageAddressTU_A();

    const oni::c8 *
    getEnumStorageAddressTU_B();

    const oni::c8 *
    getEnumStorageAddressTU_C();
}