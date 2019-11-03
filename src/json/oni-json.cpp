#include <oni-core/json/oni-json.h>

#include <fstream>
#include <istream>

namespace oni {
    std::optional<rapidjson::Document>
    readJson(const oni::FilePath &fp) {
        // TODO: Pre-load all the jsons
        std::ifstream jsonStream(fp.getFullPath());
        if (!jsonStream.is_open()) {
            return {};
        }

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());

        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            return {};
        }

        if (!document.IsObject()) {
            return {};
        }

        return document;
    }
}