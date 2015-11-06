#pragma once

namespace df3d {

class Node;
class Camera;
class Material;
class RenderQueue;
class RenderStats;

class DF3D_DLL Scene
{
    shared_ptr<Material> m_postProcessMaterial;
    shared_ptr<Node> m_root;
    shared_ptr<Camera> m_camera;

    glm::vec3 m_ambientLight = glm::vec3(1.0f, 1.0f, 1.0f);
    float m_fogDensity = 0.0f;
    glm::vec3 m_fogColor;

public:
    Scene();
    ~Scene();

    void setAmbientLight(float ra, float ga, float ba);
    glm::vec3 getAmbientLight() const;

    void setFog(float density, const glm::vec3 &color);
    void setFog(float density, float r, float g, float b);
    float getFogDensity() const;
    glm::vec3 getFogColor() const;

    void setCamera(shared_ptr<Camera> camera) { m_camera = camera; }
    shared_ptr<Camera> getCamera() const { return m_camera; }

    void setPostProcessMaterial(shared_ptr<Material> material);
    shared_ptr<Material> getPostProcessMaterial() const { return m_postProcessMaterial; }

    shared_ptr<Node> getRoot() const { return m_root; }

    void collectStats(RenderStats *stats);
    void collectRenderOperations(RenderQueue *ops);

    shared_ptr<Node> getChildByName(const std::string &name) const;
    void addChild(shared_ptr<Node> child);
    void removeChild(shared_ptr<Node> child);
    void removeChild(const std::string &name);
    void removeAllChildren();
};

}
