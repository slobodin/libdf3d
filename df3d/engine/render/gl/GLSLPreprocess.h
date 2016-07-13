#pragma once

namespace df3d {

class DF3D_DLL GLSLPreprocess
{
public:
    static std::string preprocess(const std::string &input, const std::string &shaderPath);
    static std::string preprocess(const std::string &input);
};

}
