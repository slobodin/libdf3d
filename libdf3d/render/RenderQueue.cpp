#include "df3d_pch.h"
#include "RenderQueue.h"

#include "RenderOperation.h"

namespace df3d { namespace render {

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
    std::sort(litOpaqueOperations.begin(), litOpaqueOperations.end(), sort_by_material_pred);
    std::sort(notLitOpaqueOperations.begin(), notLitOpaqueOperations.end(), sort_by_material_pred);
    std::sort(transparentOperations.begin(), transparentOperations.end(), sort_by_material_pred);
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

} }
