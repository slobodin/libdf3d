#include "RenderQueue.h"

#include "RenderOperation.h"
#include <df3d/engine/EngineController.h>
#include <df3d/engine/3d/Camera.h>
#include <df3d/game/World.h>

namespace df3d {

void RenderQueue::sort()
{
    auto cameraDir = svc().defaultWorld().getCamera()->getDir();
    auto cameraPos = svc().defaultWorld().getCamera()->getPosition();

    auto &transparentOps = rops[RQ_BUCKET_TRANSPARENT];

    std::sort(transparentOps.begin(), transparentOps.end(), [&cameraDir, &cameraPos](const RenderOperation &a, const RenderOperation &b) {
        auto d1 = glm::dot(cameraDir, glm::vec3(a.worldTransform[3]) - cameraPos);
        auto d2 = glm::dot(cameraDir, glm::vec3(b.worldTransform[3]) - cameraPos);
        return d1 > d2;
    });

    auto &ops2D = rops[RQ_BUCKET_2D];

    std::sort(ops2D.begin(), ops2D.end(), [](const RenderOperation &a, const RenderOperation &b) {
        return a.z < b.z;
    });
}

void RenderQueue::clear()
{
    for (auto &op : rops)
        op.clear();

    for (auto &l : lights)
        l = {};
}

}
