#pragma once

namespace df3d { namespace render {

class Texture;
class GpuProgram;
class GpuProgramUniform;

struct Sampler
{
    shared_ptr<render::Texture> texture;
    std::string name;
    // TODO:
    // texture unit.
};

struct DF3D_DLL RenderPassParam
{
    std::string name;
    // Only floats for now.
    float value = 0.0f;

    void updateTo(GpuProgram *program);

private:
    GpuProgramUniform *m_uniform = nullptr;
};

class DF3D_DLL RenderPass : boost::noncopyable
{
public:
    enum WindingOrder
    {
        WO_CW,
        WO_CCW
    };

    enum PolygonMode
    {
        PM_FILL,
        PM_WIRE
    };

    enum FaceCullMode
    {
        FCM_NONE,
        FCM_FRONT,
        FCM_BACK,
        FCM_FRONT_AND_BACK
    };

    enum BlendingMode
    {
        BM_NONE,
        BM_ADDALPHA,
        BM_ALPHA,
        BM_ADD
    };

private:
    std::string m_name;

    //! GPU program params.
    shared_ptr<GpuProgram> m_gpuProgram;
    //! Textures.
    std::vector<Sampler> m_samplers;
    //! Shader params.
    std::vector<RenderPassParam> m_passParams;

    bool m_isTransparent = false;
    bool m_lightingEnabled = false;

    //! Light params.
    glm::vec4 m_ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    glm::vec4 m_diffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 m_specularColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 m_emissiveColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_shininess = 64.0f;

    //! Front face winding order.
    WindingOrder m_frontFaceWo = WO_CCW;
    //! Face culling mode.
    FaceCullMode m_faceCullMode = FCM_BACK;
    //! Polygon fill mode.
    PolygonMode m_polygonMode = PM_FILL;
    //! Blending.
    BlendingMode m_blendMode = BM_NONE;

    bool m_depthTest = true;
    bool m_depthWrite = true;

    // TODO:
    // Blend factor src dst.

public:
    //! Creates a pass with default parameters.
    RenderPass(const std::string &name = "");
    ~RenderPass();

    void setGpuProgram(shared_ptr<GpuProgram> newProgram);
    shared_ptr<GpuProgram> getGpuProgram() const;

    void setSampler(const char *name, shared_ptr<Texture> texture);
    std::vector<Sampler> &getSamplers();
    shared_ptr<Texture> getSampler(const char *name);

    void addPassParam(const RenderPassParam &param);
    std::vector<RenderPassParam> &getPassParams() { return m_passParams; }
    RenderPassParam *getPassParam(const char *name);

    void setAmbientColor(float ra, float ga, float ba, float aa = 1.0f);
    void setAmbientColor(const glm::vec4 &ambient);
    void setDiffuseColor(float rd, float gd, float bd, float ad = 1.0f);
    void setDiffuseColor(const glm::vec4 &diffuse);
    void setSpecularColor(float rs, float gs, float bs, float as = 1.0f);
    void setSpecularColor(const glm::vec4 &specular);
    void setEmissiveColor(float re, float ge, float be, float ae = 1.0f);
    void setEmissiveColor(const glm::vec4 &emissive);
    void setShininess(float sh);

    void setFrontFaceWinding(WindingOrder wo);
    void setFaceCullMode(FaceCullMode mode);
    void setPolygonDrawMode(PolygonMode mode);
    void setBlendMode(BlendingMode mode);
    void enableDepthTest(bool enable);
    void enableDepthWrite(bool enable);
    void enableLighting(bool enable);

    const glm::vec4 &getAmbientColor() const { return m_ambientColor; }
    const glm::vec4 &getDiffuseColor() const { return m_diffuseColor; }
    const glm::vec4 &getSpecularColor() const { return m_specularColor; }
    const glm::vec4 &getEmissiveColor() const { return m_emissiveColor; }
    float getShininess() const { return m_shininess; }

    bool isTransparent() const { return m_isTransparent; }
    bool isLit() const { return m_lightingEnabled; }
    const std::string &getName() const { return m_name; }
    bool depthTestEnabled() const { return m_depthTest; }
    bool depthWriteEnabled() const { return m_depthWrite; }

    WindingOrder getFrontFaceWinding() const { return m_frontFaceWo; }
    FaceCullMode getFaceCullMode() const { return m_faceCullMode; }
    PolygonMode getPolygonDrawMode() const { return m_polygonMode; }
    BlendingMode getBlendingMode() const { return m_blendMode; }

    static shared_ptr<RenderPass> createDebugDrawPass();
};

} }