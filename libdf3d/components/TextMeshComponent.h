#pragma once

#include "MeshComponent.h"
#include <render/RenderOperation.h>

FWD_MODULE_CLASS(gui, FontFace)

namespace df3d { namespace components {

class DF3D_DLL TextMeshComponent : public MeshComponent
{
    shared_ptr<render::RenderPass> createRenderPass();
    void onDraw(render::RenderQueue *ops);

    shared_ptr<gui::FontFace> m_font;
    render::RenderOperation m_op;

public:
    TextMeshComponent(const char *fontPath);

    void drawText(const char *text, const glm::vec3 &color, int size = 0);

    shared_ptr<NodeComponent> clone() const;

private:
    virtual void setGeometry(shared_ptr<render::MeshData> geometry) { }
    virtual shared_ptr<render::MeshData> getGeometry() { return nullptr; }
    virtual bool isGeometryValid() const { return true; }

    virtual void setMaterial(shared_ptr<render::Material> material, size_t submeshIdx) { }
    virtual shared_ptr<render::Material> getMaterial(size_t submeshIdx) { return nullptr; }
};

} }