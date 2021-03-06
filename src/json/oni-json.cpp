#include <oni-core/json/oni-json.h>

#include <fstream>
#include <istream>

#include <cereal/external/rapidjson/error/en.h>

namespace oni {
    std::optional<rapidjson::Document>
    readJson(const oni::FilePath &fp) {
        if (fp.path.empty()) {
            assert(false);
            return {};
        }

        // TODO: Pre-load all the jsons
        std::ifstream jsonStream(fp.getFullPath());
        if (!jsonStream.is_open()) {
            assert(false);
            return {};
        }

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());

        auto document = rapidjson::Document();
        auto &parsedDoc = document.Parse(jsonString.data());
        if (parsedDoc.HasParseError()) {
            printf("JSON parse error: %s file: %s position: %zu \n",
                   rapidjson::GetParseError_En(parsedDoc.GetParseError()),
                   fp.getFullPath().data(),
                   parsedDoc.GetErrorOffset());
            assert(false);
            return {};
        }

        if (!document.IsObject()) {
            printf("JSON is not an object\n");
            assert(false);
            return {};
        }

        return document;
    }

    void
    writeJson(const oni::FilePath &fp,
              const rapidjson::Document &doc) {
        rapidjson::StringBuffer compStringBuff;
        rapidjson::PrettyWriter writer(compStringBuff);
        writer.SetIndent(' ', 2);

        doc.Accept(writer);

        std::istringstream ss(compStringBuff.GetString());

        std::ofstream output(fp.getFullPath(), std::ios::binary);

        output << ss.rdbuf();
    }
}