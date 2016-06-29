#include "RenderQueue.h"

#include "RenderOperation.h"
#include <libdf3d/base/EngineController.h>
#include <libdf3d/3d/Camera.h>
#include <libdf3d/game/World.h>

namespace df3d {

// TODO:
// Kinda radix sort, when material is encoded in int.

static bool sort_by_material_pred(const RenderOperation &a, const RenderOperation &b)
{
    return a.passProps < b.passProps;
}

static bool sort_by_z_pred(const RenderOperation2D &a, const RenderOperation2D &b)
{
    return a.z < b.z;
}

void RenderQueue::sort()
{
    auto cameraDir = svc().defaultWorld().getCamera()->getDir();
    auto cameraPos = svc().defaultWorld().getCamera()->getPosition();

    std::sort(transparentOperations.begin(), transparentOperations.end(), [&cameraDir, &cameraPos](const RenderOperation &a, const RenderOperation &b) {
        auto d1 = glm::dot(cameraDir, glm::vec3(a.worldTransform[3]) - cameraPos);
        auto d2 = glm::dot(cameraDir, glm::vec3(b.worldTransform[3]) - cameraPos);
        return d1 > d2;
    });

    std::sort(litOpaqueOperations.begin(), litOpaqueOperations.end(), sort_by_material_pred);
    std::sort(notLitOpaqueOperations.begin(), notLitOpaqueOperations.end(), sort_by_material_pred);
    std::sort(debugDrawOperations.begin(), debugDrawOperations.end(), sort_by_material_pred);
    std::sort(sprite2DOperations.begin(), sprite2DOperations.end(), sort_by_z_pred);
}

void RenderQueue::clear()
{
    notLitOpaqueOperations.clear();
    litOpaqueOperations.clear();
    transparentOperations.clear();
    debugDrawOperations.clear();
    sprite2DOperations.clear();
    lights.clear();
}

}
