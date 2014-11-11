#pragma once

namespace df3d { namespace render {

class Shader
{
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
public:
    Shader(Type type = Type::UNDEFINED);
    ~Shader();

    bool compile();

    bool compiled();

    void setCompiled(bool isCompiled);
    void setShaderDescriptor(unsigned int descr);
    void setShaderData(const std::string &data);
    void setShaderData(const char **data, size_t lnCount);
    void setType(Type type);

    Type getType() const;
    unsigned int getDescriptor() const;

    // FIXME:
    static shared_ptr<Shader> createFromFile(const char *filePath);
};

} }