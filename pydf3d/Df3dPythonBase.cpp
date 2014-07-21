#include "stdafx.h"

df3d::scene::SceneManager *df3d_getSceneManger() 
{ 
    return df3d::base::Controller::getInstance()->getSceneManager(); 
}

void df3d_quit()
{ 
    g_engineController->requestShutdown(); 
}

std::string df3d_getFileDataAsString(const char *path)
{
    auto file = g_fileSystem->openFile(path);
    if (!file || !file->valid())
        return "";

    std::string filedata(file->getSize(), 0);
    file->getRaw(&filedata[0], file->getSize());

    return filedata;
}

int df3d_viewportWidth() 
{ 
    return g_engineController->getViewport()->width(); 
}

int df3d_viewportHeight() 
{ 
    return g_engineController->getViewport()->height(); 
}

void df3d_loadResource(const char *path)
{
    g_resourceManager->loadResource(path);
}

void bindDf3dBase()
{
    using namespace boost::python;

    def("scene_mgr", df3d_getSceneManger, return_value_policy<reference_existing_object>());
    def("exit", df3d_quit);
    def("get_file_data", df3d_getFileDataAsString);
    def("viewport_width", df3d_viewportWidth);
    def("viewport_height", df3d_viewportHeight);
    def("load_resource", df3d_loadResource);
}