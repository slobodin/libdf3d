// TODO: generate this exec.
#include <df3d/df3d.h>
#include "glfwApplication.h"

int main(int agrc, char **argv) try
{
    DFLOG_MESS("main started");

    df3d::platform_impl::glfwAppRun();

    DFLOG_MESS("main finished");

    return 0;
}
catch (std::exception &e)
{
    DFLOG_CRITICAL("Fatal exception occurred: '%s'", e.what());
    return 1;
}
