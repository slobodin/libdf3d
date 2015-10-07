#pragma once

#include <json/json.h>

namespace df3d { namespace utils { namespace json {

DF3D_DLL Json::Value fromFile(const std::string &path);
DF3D_DLL Json::Value fromSource(const std::string &data);

DF3D_DLL glm::vec2 getOrDefault(const Json::Value &v, const glm::vec2 &defVal = glm::vec2());
DF3D_DLL glm::vec3 getOrDefault(const Json::Value &v, const glm::vec3 &defVal = glm::vec3());
DF3D_DLL glm::vec4 getOrDefault(const Json::Value &v, const glm::vec4 &defVal = glm::vec4());
DF3D_DLL float getOrDefault(const Json::Value &v, float defVal = 0.0f);
DF3D_DLL bool getOrDefault(const Json::Value &v, bool defVal = false);
DF3D_DLL std::string getOrDefault(const Json::Value &v, const std::string &defVal = "");
DF3D_DLL int getOrDefault(const Json::Value &v, int defVal = 0);
DF3D_DLL size_t getOrDefault(const Json::Value &v, size_t defVal = 0);

DF3D_DLL std::vector<std::string> toStringArray(const Json::Value &v);

DF3D_DLL Json::Value toJson(const glm::vec3 &v);
DF3D_DLL Json::Value toJson(const glm::vec4 &v);

} } }

template<typename T>
void operator>> (const Json::Value &value, T &to)
{
    to = df3d::utils::json::getOrDefault(value, to);
}
