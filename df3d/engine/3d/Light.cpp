#include "Light.h"

namespace df3d {

Light::Light(Type type)
{
    if (type != Type::DIRECTIONAL)
    {
        // FIXME:
        // Support other light types!
        DFLOG_WARN("Can not create light component. Unsupported light type");
        throw std::runtime_error("Not implemented!");
    }

    m_type = type;
}

}
