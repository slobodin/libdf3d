#pragma once

namespace df3d {

class ResourceDataSource;

namespace JsonUtils
{
    template <typename T>
    struct GetHelper {};

    template<>
    struct GetHelper<bool>
    {
        static bool getValue(const Json::Value &v) { return v.asBool(); }
    };

    template<>
    struct GetHelper<int32_t>
    {
        static int getValue(const Json::Value &v) { return static_cast<int32_t>(v.asInt()); }
    };

    template<>
    struct GetHelper<uint32_t>
    {
        static int getValue(const Json::Value &v) { return static_cast<uint32_t>(v.asUInt()); }
    };

    template<>
    struct GetHelper<int64_t>
    {
        static int64_t getValue(const Json::Value &v) { return static_cast<int64_t>(v.asInt64()); }
    };

    template<>
    struct GetHelper<uint64_t>
    {
        static uint64_t getValue(const Json::Value &v) { return static_cast<uint64_t>(v.asUInt64()); }
    };

    template<>
    struct GetHelper<float>
    {
        static float getValue(const Json::Value &v) { return v.asFloat(); }
    };

    template<>
    struct GetHelper<std::string>
    {
        static std::string getValue(const Json::Value &v) { return v.asString(); }
    };

    template<>
    struct GetHelper<glm::vec2>
    {
        static glm::vec2 getValue(const Json::Value &v)
        {
            DF3D_ASSERT(v.isArray());
            return{ v[0u].asFloat(), v[1u].asFloat() };
        }
    };

    template<>
    struct GetHelper<glm::ivec2>
    {
        static glm::ivec2 getValue(const Json::Value &v)
        {
            DF3D_ASSERT(v.isArray());
            return{ v[0u].asInt(), v[1u].asInt() };
        }
    };

    template<>
    struct GetHelper<glm::vec3>
    {
        static glm::vec3 getValue(const Json::Value &v)
        {
            DF3D_ASSERT(v.isArray());
            return{ v[0u].asFloat(), v[1u].asFloat(), v[2u].asFloat() };
        }
    };

    template<>
    struct GetHelper<glm::vec4>
    {
        static glm::vec4 getValue(const Json::Value &v)
        {
            DF3D_ASSERT(v.isArray());
            return{ v[0u].asFloat(), v[1u].asFloat(), v[2u].asFloat(), v[3u].asFloat() };
        }
    };

    template<>
    struct GetHelper<Id>
    {
        static Id getValue(const Json::Value &v)
        {
            DF3D_ASSERT(v.isString());
            return Id(v.asCString());
        }
    };

    Json::Value fromFile(const char *path);
    Json::Value fromFile(ResourceDataSource &dataSource);
    Json::Value fromString(const std::string &data);

    template<typename T>
    inline T get(const Json::Value &v, const char *key, const T &defVal = {})
    {
        auto found = v.find(key, key + strlen(key));
        if (found)
            return GetHelper<T>::getValue(*found);
        return defVal;
    }
};

}
