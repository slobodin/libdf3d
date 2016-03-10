#pragma once

#include <libdf3d/resources/Resource.h>
#include "RenderCommon.h"

namespace df3d {

class GpuProgram : public Resource
{
    friend class GpuProgramManualLoader;

    GpuProgramDescriptor m_descriptor;

    GpuProgram(GpuProgramDescriptor descr);

public:
    ~GpuProgram();

    GpuProgramDescriptor getDescriptor() const { return m_descriptor; }
};

class GpuProgramManualLoader : public ManualResourceLoader
{
    std::string m_resourceGuid;
    std::string m_vertexData;
    std::string m_fragmentData;

    std::string m_vertexShaderPath, m_fragmentShaderPath;

    ShaderDescriptor createShaderFromFile(const std::string &path) const;
    ShaderDescriptor createShaderFromString(const std::string &data, ShaderType shaderType) const;

public:
    GpuProgramManualLoader(const std::string &guid, const std::string &vertexData, const std::string &fragmentData);
    GpuProgramManualLoader(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);

    GpuProgram* load() override;
};

}
