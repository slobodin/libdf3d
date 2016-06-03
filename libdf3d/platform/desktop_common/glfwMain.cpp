// TODO: generate this exec.
#include <libdf3d/df3d.h>
#include "glfwApplication.h"

extern "C" void df3dInitialized();

int main(int agrc, char **argv) try
{
    df3dInitialized();

    df3d::platform_impl::glfwAppRun();

    DFLOG_DEBUG("main finished");

    std::cout << "main finished" << std::endl;

    return 0;
}
catch (std::exception &e)
{
    DFLOG_CRITICAL("Fatal exception occurred: '%s'", e.what());
    return 1;
}
