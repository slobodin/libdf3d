#pragma once

FWD_MODULE_CLASS(render, MeshData)

namespace df3d { namespace scene {

class DF3D_DLL BoundingVolume
{
public:
    virtual ~BoundingVolume() {}

    virtual void reset() = 0;
    virtual void updateBounds(const glm::vec3 &point) = 0;

    virtual bool isValid() const = 0;

    void constructFromGeometry(shared_ptr<render::MeshData> geometry);
};

} }