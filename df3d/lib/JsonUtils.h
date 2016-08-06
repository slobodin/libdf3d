#pragma once

#include <json/json.h>

namespace df3d {

class DF3D_DLL JsonUtils
{
public:
    // TODO: should not rely on engine.
    static Json::Value fromFile(const std::string &path);
    static Json::Value fromSource(const std::string &data);

    static glm::vec2 getOrDefault(const Json::Value &v, const glm::vec2 &defVal = {});
    static glm::vec3 getOrDefault(const Json::Value &v, const glm::vec3 &defVal = {});
    static glm::vec4 getOrDefault(const Json::Value &v, const glm::vec4 &defVal = {});
    static float getOrDefault(const Json::Value &v, float defVal = 0.0f);
    static bool getOrDefault(const Json::Value &v, bool defVal = false);
    static std::string getOrDefault(const Json::Value &v, const std::string &defVal = "");
    static int getOrDefault(const Json::Value &v, int defVal = 0);
    static size_t getOrDefault(const Json::Value &v, size_t defVal = 0);

    static std::vector<std::string> toStringArray(const Json::Value &v);

    static Json::Value toJson(const glm::vec3 &v);
    static Json::Value toJson(const glm::vec4 &v);
};

}

template<typename T>
void operator>> (const Json::Value &value, T &to)
{
    to = df3d::JsonUtils::getOrDefault(value, to);
}
