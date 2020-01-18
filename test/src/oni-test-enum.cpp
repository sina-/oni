#include <oni-test/oni-test-enum.h>

#include <oni-core/util/oni-util-enum.h>

#include <oni-test/oni-test-enum-storage.h>

ONI_ENUM_DEF(Two_Distinct_Elements, { 0, "first" }, { 1, "second" })
ONI_ENUM_DEF(Two_Duplicate_Ids, { 2, "first two" }, { 2, "second two" })

struct Base_Enum : public oni::Enum {
};

ONI_ENUM_DEF_WITH_BASE(Derived_Enum, Base_Enum, { 12, "twelve" }, { 13, "thirteen" })


namespace {
    void
    testDefaultInit() {
        constexpr auto first = Two_Distinct_Elements{};
        static_assert(first.id == 0, "expecting id == 0");
        static_assert(first.name == oni::HashedString{}, "expecting name == {}");
    }

    void
    testDistinctElements() {
        constexpr auto first = Two_Distinct_Elements::GET("first");
        constexpr auto second = Two_Distinct_Elements::GET("second");

        static_assert(second.id == 1, "expecting id == 1");
        static_assert(first.id == 0, "expecting id == 0");

        static_assert(first.name == "first", "expecting name == first");
        static_assert(second.name == "second", "expecting name == second");
    }

    void
    testDuplicateIds() {
        constexpr auto first = Two_Duplicate_Ids::GET("first two");
        constexpr auto second = Two_Duplicate_Ids::GET("second two");

        static_assert(second.id == 2, "expecting id == 2");
        static_assert(first.id == 2, "expecting id == 2");

        static_assert(first.name == "first two", "expecting name == first two");
        static_assert(second.name == "second two", "expecting name == second two");
    }

    void
    testNotFound() {
        bool pass = false;
        auto invalid = Two_Distinct_Elements{};
        try {
            invalid = Two_Distinct_Elements::GET("INVALID");
        }
        catch (std::logic_error &e) {
            pass = true;
        }
        assert(pass);
    }

    void
    testUnequalTypes() {
        assert(typeid(Two_Duplicate_Ids) != typeid(Two_Distinct_Elements));
    }

    void
    testEqualTypes() {
        assert(typeid(Two_Distinct_Elements) == typeid(Two_Distinct_Elements));
    }

    void
    testDerivedEnum() {
        static_assert(std::is_base_of_v<Base_Enum, Derived_Enum>, "expected parent child relationship");
        static_assert(!std::is_base_of_v<Derived_Enum, Base_Enum>, "did not expected parent child relationship");
    }

    void
    testRuntimeInitValid() {
        constexpr auto hash = oni::HashedString::makeHashFromLiteral("second");
        auto enumValue = Two_Distinct_Elements{};
        enumValue.runtimeInit(hash);

        assert(enumValue.id == 1);
        assert(enumValue.name == "second");
    }

    void
    testRuntimeInitInvalid() {
        constexpr auto hash = oni::HashedString::makeHashFromLiteral("WHAT");
        auto enumValue = Two_Distinct_Elements{};
        enumValue.runtimeInit(hash);

        assert(enumValue.id == 0);
        assert(enumValue.id != 1);
        assert(enumValue.name != "second");
        assert(enumValue.name == "__INVALID__");
    }

    void
    testSize() {
        static_assert(Derived_Enum::size() == 2, "expected 2");
    }

    void
    testConstexprIf() {
        if constexpr(Two_Duplicate_Ids::GET("first two").name == "first two") {
            assert(true);
        } else {
            assert(false);
        }
    }

    void
    testStorage() {
        auto addressA = oni::getEnumStorageAddressTU_A();
        auto addressB = oni::getEnumStorageAddressTU_B();
        auto addressC = oni::getEnumStorageAddressTU_C();

        assert(addressA == addressB);
        assert(addressA != addressC);
        assert(addressB != addressC);
    }
}

namespace oni {
    void
    OniTestEnum::run() {
        testDefaultInit();
        testDistinctElements();
        testDuplicateIds();
        testNotFound();
        testUnequalTypes();
        testEqualTypes();
        testDerivedEnum();
        testRuntimeInitValid();
        testRuntimeInitInvalid();
        testSize();
        testConstexprIf();
        testStorage();
    }
}
