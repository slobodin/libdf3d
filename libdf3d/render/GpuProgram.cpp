#include "GpuProgram.h"

#include <libdf3d/base/EngineController.h>
#include <libdf3d/render/RenderManager.h>
#include <libdf3d/render/IRenderBackend.h>

namespace df3d {

GpuProgram::GpuProgram(GpuProgramDescriptor descr)
    : m_descriptor(descr)
{
    assert(m_descriptor.valid());
}

GpuProgram::~GpuProgram()
{
    svc().renderManager().getBackend().destroyGpuProgram(m_descriptor);
}

GpuProgramManualLoader::GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData)
    : m_resourceGuid(guid),
    m_vertexData(vertexData),
    m_fragmentData(fragmentData)
{

}

GpuProgramManualLoader::GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
    : m_resourceGuid(vertexShaderPath + ";" + fragmentShaderPath),
    m_vertexShaderPath(vertexShaderPath),
    m_fragmentShaderPath(fragmentShaderPath)
{
    assert(vertexShaderPath != fragmentShaderPath);
}

GpuProgram* GpuProgramManualLoader::load()
{
    // TODO_render
    /*

    shared_ptr<Shader> vertexShader, fragmentShader;

    if (!m_vertexShaderPath.empty() && !m_fragmentShaderPath.empty())
    {
        vertexShader = Shader::createFromFile(m_vertexShaderPath);
        fragmentShader = Shader::createFromFile(m_fragmentShaderPath);
    }
    else
    {
        vertexShader = Shader::createFromString(m_vertexData, Shader::Type::VERTEX);
        fragmentShader = Shader::createFromString(m_fragmentData, Shader::Type::FRAGMENT);
    }

    auto program = new GpuProgram({ vertexShader, fragmentShader });

    program->setGUID(m_resourceGuid);

    return program;*/
    return nullptr;
}

}
