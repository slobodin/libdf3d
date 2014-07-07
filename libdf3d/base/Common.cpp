#include "df3d_pch.h"
#include "Common.h"

namespace df3d {

glm::vec3 jsonGetVec3(const Json::Value &root, glm::vec3 defVal)
{
    if (root.empty())
    {
        return defVal;
    }

    return glm::vec3((float)root[0u].asDouble(), (float)root[1u].asDouble(), (float)root[2u].asDouble());
}

glm::vec4 jsonGetVec4(const Json::Value &root, glm::vec4 defVal)
{
    if (root.empty())
    {
        return defVal;
    }

    return glm::vec4((float)root[0u].asDouble(), (float)root[1u].asDouble(), (float)root[2u].asDouble(), (float)root[3u].asDouble());
}

DF3D_DLL std::string glmVecToString(const glm::vec3 &v)
{
    std::ostringstream ss;
    ss.precision(4);

    ss << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";

    return ss.str();
}

}