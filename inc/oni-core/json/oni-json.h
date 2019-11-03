#pragma once

#include <cassert>

// NOTE: For cereal to not throw exceptions and disable stupid noexcept constructor that throws in debug!
#define JSON_NOEXCEPTION

#include <cereal/archives/json.hpp>
#include <oni-core/util/oni-util-structure.h>

namespace oni {
    std::optional<rapidjson::Document>
    readJson(const oni::FilePath &fp);
}