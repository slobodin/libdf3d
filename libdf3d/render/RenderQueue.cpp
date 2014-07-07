#include "df3d_pch.h"
#include "RenderQueue.h"

#include "RenderOperation.h"

namespace df3d { namespace render {

static bool sort_pred(const RenderOperation &a, const RenderOperation &b)
{
    return a.passProps.get() < b.passProps.get();
}

void RenderQueue::sort()
{
    std::sort(litOpaqueOperations.begin(), litOpaqueOperations.end(), sort_pred);
    std::sort(notLitOpaqueOperations.begin(), notLitOpaqueOperations.end(), sort_pred);
    std::sort(transparentOperations.begin(), transparentOperations.end(), sort_pred);
    std::sort(debugDrawOperations.begin(), debugDrawOperations.end(), sort_pred);
}

void RenderQueue::clear()
{
    notLitOpaqueOperations.clear();
    litOpaqueOperations.clear();
    transparentOperations.clear();
    debugDrawOperations.clear();
    lights.clear();
}

} }