#include "GLSLPreprocess.h"

#include <cctype>
#include <df3d/engine/EngineController.h>
#include <df3d/engine/io/DataSource.h>
#include <df3d/engine/io/DefaultFileSystem.h>
#include <df3d/engine/io/FileSystemHelpers.h>
#include <df3d/engine/render/MaterialLib.h>
#include <df3d/lib/Utils.h>

namespace df3d {

static std::string ShaderIfdefsParse(const std::string &shaderData)
{
    /*
    const std::string IFDEF_DIRECTIVE = "#ifdef";
    const std::string ENDIF_DIRECTIVE = "#endif";
    std::string res = shaderData;

    size_t lastFoundIfdef = res.find(IFDEF_DIRECTIVE);
    while (lastFoundIfdef != std::string::npos)
    {
        auto found = std::find_if(res.begin() + lastFoundIfdef + IFDEF_DIRECTIVE.size(), res.end(), [](char ch) { return !std::isspace(ch); });
        if (found == res.end())
            return shaderData;

        auto foundEndif = res.find_first_of('#', std::distance(res.begin(), found));
        if (foundEndif == std::string::npos)
            return shaderData;

        auto defineEnd = std::find_if(found, res.begin() + foundEndif, [](char ch) { return std::isspace(ch); });

        std::string defineToken = std::string(found, defineEnd);

        if (!utils::contains_key(MaterialLib::SHADER_DEFINES, defineToken))
        {
            auto beginErase = res.begin() + lastFoundIfdef;
            auto endErase = res.begin() + foundEndif + ENDIF_DIRECTIVE.size();

            res.erase(beginErase, endErase);
        }
        else
        {
            auto beginErase = res.begin() + lastFoundIfdef;

            res.erase(beginErase, defineEnd);

            foundEndif = res.find_first_of('#');        // Should be #endif

            res.erase(foundEndif, ENDIF_DIRECTIVE.size());
        }

        lastFoundIfdef = res.find(IFDEF_DIRECTIVE, 0);
    }*/

    //return res;
    return "";
}

static std::string ShaderPreprocess(const std::string &shaderData)
{
#ifdef DF3D_DESKTOP
    std::string versionPrefix = "#version 110\n";
#else
    std::string versionPrefix = "";
#endif

    std::string precisionPrefix = "#ifdef GL_ES\n"
        "#define LOWP lowp\n"
        "#define MEDIUMP mediump\n"
        "precision highp float;\n"
        "#else\n"
        "#define LOWP\n"
        "#define MEDIUMP\n"
        "#endif\n";

    return versionPrefix + precisionPrefix + shaderData;
}

static std::string ShaderPreprocessInclude(std::string shaderData, const std::string &shaderFilePath)
{
    const std::string shaderDirectory = FileSystemHelpers::getFileDirectory(shaderFilePath);
    const std::string INCLUDE_DIRECTIVE = "#include";
    const size_t INCLUDE_DIRECTIVE_LEN = INCLUDE_DIRECTIVE.size();

    size_t found = shaderData.find(INCLUDE_DIRECTIVE, 0);
    while (found != std::string::npos)
    {
        auto start = shaderData.find('\"', found + INCLUDE_DIRECTIVE_LEN);
        auto end = shaderData.find('\"', start + 1);

        if (start == end || start == std::string::npos || end == std::string::npos)
        {
            DFLOG_WARN("Failed to preprocess a shader: invalid include directive");
            return shaderData;
        }

        auto fileToInclude = shaderData.substr(start + 1, end - start - 1);
        if (fileToInclude.empty())
        {
            DFLOG_WARN("Failed to preprocess a shader: empty include path");
            return shaderData;
        }

        fileToInclude = FileSystemHelpers::pathConcatenate(shaderDirectory, fileToInclude);
        auto file = svc().fileSystem().open(fileToInclude);
        if (!file)
        {
            DFLOG_WARN("Failed to preprocess a shader: file %s is not found", fileToInclude.c_str());
            return shaderData;
        }

        std::string includeData(file->getSize(), 0);
        file->read(&includeData[0], includeData.size());

        shaderData.replace(found, end - found + 1, includeData);

        found = shaderData.find(INCLUDE_DIRECTIVE, 0);
    }

    return shaderData;
}

std::string GLSLPreprocess::preprocess(const std::string &input, const std::string &shaderPath)
{
    return ShaderPreprocess(ShaderPreprocessInclude(input, shaderPath));
}

std::string GLSLPreprocess::preprocess(const std::string &input)
{
    return ShaderPreprocess(input);
}

}
