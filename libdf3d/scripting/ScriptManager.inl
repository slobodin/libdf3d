#pragma once

// FIXME:
// Do not want to expose python to client.
// Create engine define USES_PYTHON, create project with this define.
#define BOOST_PYTHON_STATIC_LIB
#include <boost/python.hpp>

#include <resources/FileSystem.h>

namespace df3d { namespace scripting {

template<typename T>
T ScriptManager::getPythonObject(const char *pyFile, const char *pyObjName)
{
    auto fullp = createAndAddFullPath(pyFile);

    using namespace boost::python;

    auto moduleName = resources::FileSystem::getFilenameWithoutExtension(fullp);
    auto module = import(moduleName.c_str());
    auto moduleNamespace(module.attr("__dict__"));

    return moduleNamespace[pyObjName];
}

} }