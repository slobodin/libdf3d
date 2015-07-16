#pragma once

namespace df3d { namespace render {

class GpuProgram;

class Shader
{
    friend class GpuProgram;
public:
    enum class Type
    {
        UNDEFINED,
        VERTEX,
        FRAGMENT
    };

private:
    Type m_type = Type::UNDEFINED;
    unsigned int m_shaderDescriptor = 0;
    std::string m_shaderData;

    bool m_isCompiled = false;

    void createGLShader();
    //! Shader code preprocessing.
    static std::string preprocess(const std::string &shaderData);
    static std::string preprocessInclude(std::string shaderData, const std::string &shaderFilePath);

    void setCompiled(bool isCompiled) { m_isCompiled = isCompiled; }
    void setShaderData(const std::string &data);
    void setType(Type type) { m_type = type; }

    bool compile();

public:
    Shader(Type type = Type::UNDEFINED);
    ~Shader();

    Type getType() const { return m_type; }
    unsigned int getDescriptor() const { return m_shaderDescriptor; }
    bool compiled() { return m_isCompiled; }

    static shared_ptr<Shader> createFromFile(const char *filePath);
    static shared_ptr<Shader> createFromString(const char *shaderData, Type type);
};

} }
