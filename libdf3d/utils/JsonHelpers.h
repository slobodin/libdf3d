#pragma once

#include <json/json.h>

namespace df3d { namespace utils {

DF3D_DLL Json::Value jsonLoadFromFile(const char *path);
DF3D_DLL Json::Value jsonLoadFromSource(const char *data);

DF3D_DLL glm::vec2 jsonGetValueWithDefault(const Json::Value &v, const glm::vec2 &defVal = glm::vec2());
DF3D_DLL glm::vec3 jsonGetValueWithDefault(const Json::Value &v, const glm::vec3 &defVal = glm::vec3());
DF3D_DLL glm::vec4 jsonGetValueWithDefault(const Json::Value &v, const glm::vec4 &defVal = glm::vec4());
DF3D_DLL float jsonGetValueWithDefault(const Json::Value &v, float defVal = 0.0f);
DF3D_DLL bool jsonGetValueWithDefault(const Json::Value &v, bool defVal = false);
DF3D_DLL std::string jsonGetValueWithDefault(const Json::Value &v, const std::string &defVal = "");
DF3D_DLL int jsonGetValueWithDefault(const Json::Value &v, int defVal = 0);
DF3D_DLL size_t jsonGetValueWithDefault(const Json::Value &v, size_t defVal = 0);

DF3D_DLL std::vector<std::string> jsonArrayAsStrings(const Json::Value &v);

DF3D_DLL Json::Value glmToJson(const glm::vec3 &v);
DF3D_DLL Json::Value glmToJson(const glm::vec4 &v);

} }

template<typename T>
void operator>> (const Json::Value &value, T &to)
{
    to = df3d::utils::jsonGetValueWithDefault(value, to);
}
