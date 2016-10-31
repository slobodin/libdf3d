#pragma once

namespace df3d {

class BoundingVolume
{
public:
    virtual ~BoundingVolume() = default;

    virtual void reset() = 0;
    virtual void updateBounds(const glm::vec3 &point) = 0;

    virtual bool isValid() const = 0;
};

}
