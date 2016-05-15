#include "JsonUtils.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>

namespace df3d { namespace utils { namespace json {

Json::Value fromFile(const std::string &path)
{
    std::string buffer;

    if (auto fileSource = svc().fileSystem().openFile(path))
    {
        buffer.resize(fileSource->getSizeInBytes());
        fileSource->getRaw(&buffer[0], buffer.size());
    }
    else
        DFLOG_WARN("Couldn't load json configs from %s", path.c_str());

    return fromSource(buffer);
}

Json::Value fromSource(const std::string &data)
{
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(data, root))
    {
        DFLOG_WARN("Failed to parse json. Error: %s", reader.getFormattedErrorMessages().c_str());
        return{};
    }

    return root;
}

glm::vec2 getOrDefault(const Json::Value &v, const glm::vec2 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec2(v[0u].asFloat(), v[1u].asFloat());
}

glm::vec3 getOrDefault(const Json::Value &v, const glm::vec3 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec3(v[0u].asFloat(), v[1u].asFloat(), v[2u].asFloat());
}

glm::vec4 getOrDefault(const Json::Value &v, const glm::vec4 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec4(v[0u].asFloat(), v[1u].asFloat(), v[2u].asFloat(), v[3u].asFloat());
}

float getOrDefault(const Json::Value &v, float defVal)
{
    if (v.empty())
        return defVal;
    return v.asFloat();
}

bool getOrDefault(const Json::Value &v, bool defVal)
{
    if (v.empty())
        return defVal;
    return v.asBool();
}

std::string getOrDefault(const Json::Value &v, const std::string &defVal)
{
    if (v.empty())
        return defVal;
    return v.asString();
}

int getOrDefault(const Json::Value &v, int defVal)
{
    if (v.empty())
        return defVal;
    return v.asInt();
}

size_t getOrDefault(const Json::Value &v, size_t defVal)
{
    if (v.empty())
        return defVal;
    return v.asUInt();
}

std::vector<std::string> toStringArray(const Json::Value &v)
{
    std::vector<std::string> result;
    if (v.empty())
        return result;

    for (const auto &val : v)
        result.push_back(val.asString());
    return result;
}

Json::Value toJson(const glm::vec3 &v)
{
    Json::Value result(Json::arrayValue);
    result.append(Json::Value(v.x));
    result.append(Json::Value(v.y));
    result.append(Json::Value(v.z));

    return result;
}

Json::Value toJson(const glm::vec4 &v)
{
    Json::Value result(Json::arrayValue);
    result.append(Json::Value(v.x));
    result.append(Json::Value(v.y));
    result.append(Json::Value(v.z));
    result.append(Json::Value(v.w));

    return result;
}

} } }
