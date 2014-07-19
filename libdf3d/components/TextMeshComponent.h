#pragma once

#include "MeshComponent.h"
#include <render/RenderOperation.h>
// FIXME:
// Do not want to expose SDL ttf to client.
#include <SDL_ttf.h>

FWD_MODULE_CLASS(render, RenderPass)

namespace df3d { namespace components {

class DF3D_DLL TextMeshComponent : public MeshComponent
{
    TTF_Font *m_font = nullptr;
    render::RenderOperation m_op;

    shared_ptr<render::RenderPass> createRenderPass();
    shared_ptr<render::VertexBuffer> createQuad(float x, float y, float w, float h);
    void onDraw(render::RenderQueue *ops);

public:
    TextMeshComponent(const char *fontPath);
    ~TextMeshComponent();

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