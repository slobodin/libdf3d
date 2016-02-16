// TODO: generate this exec.
#include <stdexcept>
#include <iostream>
#include "glfwApplication.h"

extern "C" void df3dInitialized();

int main(int agrc, char **argv) try
{
    df3dInitialized();

    df3d::platform_impl::glfwAppRun();

    std::cout << "main finished" << std::endl;

    return 0;
}
catch (std::exception &e)
{
    std::cout << "Fatal exception occurred: '" << e.what() << "'" << std::endl;
    return 1;
}
