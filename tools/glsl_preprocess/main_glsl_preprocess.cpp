#include <iostream>

#include <glsl-optimizer/glsl_optimizer.h>
#include <df3d/df3d.h>
#include <df3d/engine/render/gl/GLSLPreprocess.h>

int main()
{
    /*
    std::string input = "c:/dev/ships/data/glsl/normalmap.vert";

    df3d::MemoryManager::init();

    df3d::DefaultFileSystem fs;
    df3d::svc().setFileSystem(make_unique<df3d::DefaultFileSystem>());

    std::string inputFileName = input;

    auto file = fs.open(inputFileName);
    if (!file)
        throw std::runtime_error("Failed to open input file");

    std::string buf;
    buf.resize(file->getSize());
    file->read(&buf[0], file->getSize());


    buf = df3d::GLSLPreprocess::preprocess(buf, file->getPath());

    auto ctx = glslopt_initialize(kGlslTargetOpenGL);

    auto shader = glslopt_optimize(ctx, kGlslOptShaderVertex, buf.c_str(), 0);
    if (glslopt_get_status(shader)) {
        auto newSource = glslopt_get_output(shader);
        std::string res = newSource;
        int a;
        a = 2;
    }
    else {
        auto errorLog = glslopt_get_log(shader);
        std::string aa = errorLog;
        int a;
        a = 2;
    }


    */
    return 0;
}
