#pragma once

namespace df3d {

class SubMesh;

class DF3D_DLL BoundingVolume
{
public:
    virtual ~BoundingVolume() = default;

    virtual void reset() = 0;
    virtual void updateBounds(const glm::vec3 &point) = 0;

    virtual bool isValid() const = 0;

    virtual void constructFromGeometry(const std::vector<SubMesh> &submeshes);
};

}
