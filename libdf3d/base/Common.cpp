#include "df3d_pch.h"
#include "Common.h"

namespace df3d {

DF3D_DLL std::string glmVecToString(const glm::vec3 &v)
{
    std::ostringstream ss;
    ss.precision(4);

    ss << "[ " << v.x << ", " << v.y << ", " << v.z << " ]";

    return ss.str();
}

}
