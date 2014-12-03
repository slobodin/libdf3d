#include "df3d_pch.h"
#include "Common.h"

namespace df3d {

glm::vec3 jsonGetVec3(const Json::Value &root, glm::vec3 defVal)
{
    if (root.empty())
    {
        return defVal;
    }

    return glm::vec3(root[0u].asFloat(), root[1u].asFloat(), root[2u].asFloat());
}

glm::vec4 jsonGetVec4(const Json::Value &root, glm::vec4 defVal)
{
    if (root.empty())
    {
        return defVal;
    }

    return glm::vec4(root[0u].asFloat(), root[1u].asFloat(), root[2u].asFloat(), root[3u].asFloat());
}

DF3D_DLL std::string glmVecToString(const glm::vec3 &v)
{
    std::ostringstream ss;
    ss.precision(4);

    ss << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";

    return ss.str();
}

}