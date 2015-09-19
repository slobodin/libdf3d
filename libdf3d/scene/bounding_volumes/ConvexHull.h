#pragma once

#include "BoundingVolume.h"

namespace df3d { namespace scene {

class DF3D_DLL ConvexHull : public BoundingVolume
{

public:
    void reset() override;
    void updateBounds(const glm::vec3 &point) override;
    bool isValid() const override;
};

} }
