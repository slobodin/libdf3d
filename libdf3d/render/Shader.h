#pragma once

namespace df3d { namespace render {

class Shader
{
public:
    enum ShaderType
    {
        ST_UNDEFINED,
        ST_VERTEX,
        ST_FRAGMENT
    };

private:
    ShaderType m_type = ST_UNDEFINED;
    unsigned int m_shaderDescriptor = 0;
    std::string m_shaderData;

    bool m_isCompiled = false;

    void createGLShader();
public:
    Shader(ShaderType type = ST_UNDEFINED);
    ~Shader();

    bool compile();

    bool compiled();

    void setCompiled(bool isCompiled);
    void setShaderDescriptor(unsigned int descr);
    void setShaderData(const std::string &data);
    void setShaderData(const char **data, size_t lnCount);
    void setType(ShaderType type);

    ShaderType getType() const;
    unsigned int getDescriptor() const;

    // FIXME:
    static shared_ptr<Shader> createFromFile(const char *filePath);
};

} }