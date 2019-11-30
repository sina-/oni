#pragma once

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-serialization-json.h>
#include <oni-particle-editor/entities/oni-particle-editor-entities-structure.h>


namespace oni {
//    template<class Archive>
//    void
//    save(Archive &archive,
//         const PhysicalCategory &data) {
//        saveEnum(archive, "name", data);
//    }
//
//    template<class Archive>
//    void
//    load(Archive &archive,
//         PhysicalCategory &data) {
//        auto buffer = rac::PhysicalCategoryRac{};
//        loadEnum(archive, "name", buffer);
//        data = buffer;
//    }

    template<class Archive>
    void
    save(Archive &archive,
         const EntityName &data) {
        saveEnum(archive, "value", data);
    }

    template<class Archive>
    void
    load(Archive &archive,
         EntityName &data) {
        auto buffer = EntityNameEditor{};
        loadEnum(archive, "value", buffer);
        data = buffer;
    }
}

