#pragma once

#include "BoundingVolume.h"

namespace df3d {

class Allocator;

class DF3D_DLL ConvexHull : public BoundingVolume
{
    PodArray<glm::vec3> m_vertices;

public:
    ConvexHull(Allocator *allocator);
    ~ConvexHull();

    void reset() override;
    void updateBounds(const glm::vec3 &point) override;
    bool isValid() const override;

    void constructFromGeometry(const std::vector<SubMesh> &submeshes) override;

    const PodArray<glm::vec3>& getVertices() const { return m_vertices; }
};

}
