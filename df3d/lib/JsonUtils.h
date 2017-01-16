#pragma once

#include <rapidjson/document.h>

namespace df3d {

class ResourceDataSource;

namespace JsonUtils
{
    // TODO_serial: rename
    rapidjson::Document fromFile(const char *path);
    rapidjson::Document fromFile(ResourceDataSource &dataSource);
    rapidjson::Document fromString(const std::string &data);

    template<typename T>
    inline T get(const rapidjson::Value &v, const char *key, const T &defVal = {})
    {
        DF3D_ASSERT(v.IsObject());
        auto found = v.FindMember(key);
        if (found != v.MemberEnd())
            return found->value.Get<T>();
        return defVal;
    }

    template<>
    inline std::string get(const rapidjson::Value &v, const char *key, const std::string &defVal)
    {
        DF3D_ASSERT(v.IsObject());
        auto found = v.FindMember(key);
        if (found != v.MemberEnd())
            return std::string(found->value.GetString());
        return defVal;
    }

    template<>
    inline glm::vec2 get(const rapidjson::Value &v, const char *key, const glm::vec2 &defVal)
    {
        DF3D_ASSERT(v.IsObject());
        auto found = v.FindMember(key);
        if (found != v.MemberEnd())
        {
            auto arr = found->value.GetArray();
            return{ arr[0].GetFloat(), arr[1].GetFloat() };
        }
        return defVal;
    }

    template<>
    inline glm::vec3 get(const rapidjson::Value &v, const char *key, const glm::vec3 &defVal)
    {
        DF3D_ASSERT(v.IsObject());
        auto found = v.FindMember(key);
        if (found != v.MemberEnd())
        {
            auto arr = found->value.GetArray();
            return{ arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat() };
        }
        return defVal;
    }

    template<>
    inline glm::vec4 get(const rapidjson::Value &v, const char *key, const glm::vec4 &defVal)
    {
        DF3D_ASSERT(v.IsObject());
        auto found = v.FindMember(key);
        if (found != v.MemberEnd())
        {
            auto arr = found->value.GetArray();
            return{ arr[0].GetFloat(), arr[1].GetFloat(), arr[2].GetFloat(), arr[3].GetFloat() };
        }
        return defVal;
    }
};

}
