#pragma once

#include "BoundingVolume.h"

namespace df3d {

class DF3D_DLL ConvexHull : public BoundingVolume
{
    std::vector<glm::vec3> m_vertices;

public:
    ConvexHull();
    ~ConvexHull();

    void reset() override;
    void updateBounds(const glm::vec3 &point) override;
    bool isValid() const override;

    void constructFromGeometry(const std::vector<SubMesh> &submeshes) override;

    const std::vector<glm::vec3>& getVertices() const { return m_vertices; }
};

}
