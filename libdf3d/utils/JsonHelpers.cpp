#include "df3d_pch.h"
#include "JsonHelpers.h"

#include <base/Controller.h>
#include <resources/FileSystem.h>
#include <resources/FileDataSource.h>

namespace df3d { namespace utils {

Json::Value jsonLoadFromFile(const char *path)
{
    auto fileSource = g_fileSystem->openFile(path);
    if (!fileSource || !fileSource->valid())
    {
        base::glog << "Couldn't load json configs from" << path << base::logwarn;
        return Json::Value();
    }

    Json::Value root;
    Json::Reader reader;

    std::string buffer(fileSource->getSize(), 0);
    fileSource->getRaw(&buffer[0], fileSource->getSize());

    if (!reader.parse(buffer.c_str(), root))
    {
        base::glog << "Failed to parse json from" << path << ". Error:" << reader.getFormatedErrorMessages() << base::logwarn;
        return Json::Value();
    }

    return root;
}

Json::Value jsonLoadFromSource(const char *data)
{
    Json::Value root;
    Json::Reader reader;

    if (!reader.parse(data, root))
    {
        base::glog << "Failed to parse json. Error:" << reader.getFormatedErrorMessages() << base::logwarn;
        return Json::Value();
    }

    return root;
}

glm::vec2 jsonGetValueWithDefault(const Json::Value &v, const glm::vec2 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec2((float)v[0u].asDouble(), (float)v[1u].asDouble());
}

glm::vec3 jsonGetValueWithDefault(const Json::Value &v, const glm::vec3 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec3((float)v[0u].asDouble(), (float)v[1u].asDouble(), (float)v[2u].asDouble());
}

glm::vec4 jsonGetValueWithDefault(const Json::Value &v, const glm::vec4 &defVal)
{
    if (v.empty())
        return defVal;
    return glm::vec4((float)v[0u].asDouble(), (float)v[1u].asDouble(), (float)v[2u].asDouble(), (float)v[3u].asDouble());
}

float jsonGetValueWithDefault(const Json::Value &v, float defVal)
{
    if (v.empty())
        return defVal;
    return (float)v.asDouble();
}

bool jsonGetValueWithDefault(const Json::Value &v, bool defVal)
{
    if (v.empty())
        return defVal;
    return v.asBool();
}

std::string jsonGetValueWithDefault(const Json::Value &v, const std::string &defVal)
{
    if (v.empty())
        return defVal;
    return v.asString();
}

int jsonGetValueWithDefault(const Json::Value &v, int defVal)
{
    if (v.empty())
        return defVal;
    return v.asInt();
}

size_t jsonGetValueWithDefault(const Json::Value &v, size_t defVal)
{
    if (v.empty())
        return defVal;
    return v.asUInt();
}

} }