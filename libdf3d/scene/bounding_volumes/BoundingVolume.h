#pragma once

FWD_MODULE_CLASS(render, SubMesh)

namespace df3d { namespace scene {

class DF3D_DLL BoundingVolume
{
public:
    virtual ~BoundingVolume() = default;

    virtual void reset() = 0;
    virtual void updateBounds(const glm::vec3 &point) = 0;

    virtual bool isValid() const = 0;

    virtual void constructFromGeometry(const std::vector<render::SubMesh> &submeshes);
};

} }
