#include "df3d_pch.h"
#include "ScriptManager.h"

#include <resources/FileSystem.h>
#include <base/Controller.h>
#include "PythonUpdateProxy.h"
#include "PythonInputProxy.h"

namespace df3d { namespace scripting {

#if defined(__ANDROID__)

static PyObject *androidPythonLog(PyObject *self, PyObject *args)
{
    char *logstr = NULL;
    if (!PyArg_ParseTuple(args, "s", &logstr))
    {
        return NULL;
    }

    base::glog << "[python]" << logstr << base::logmess;

    Py_RETURN_NONE;
}

static PyMethodDef AndroidEmbedMethods[] = {
    { "log", androidPythonLog, METH_VARARGS, "Log on android platform" },
    { NULL, NULL, 0, NULL }
};

PyMODINIT_FUNC initPythonAndroidLog()
{
    Py_InitModule("androidembed", AndroidEmbedMethods);

    auto result = PyRun_SimpleString(
        "import sys\n" \
        "import androidembed\n" \
        "class LogFile(object):\n" \
        "    def __init__(self):\n" \
        "        self.buffer = ''\n" \
        "    def write(self, s):\n" \
        "        s = self.buffer + s\n" \
        "        lines = s.split(\"\\n\")\n" \
        "        for l in lines[:-1]:\n" \
        "            androidembed.log(l)\n" \
        "        self.buffer = lines[-1]\n" \
        "    def flush(self):\n" \
        "        return\n" \
        "sys.stdout = sys.stderr = LogFile()\n"
        );

    base::glog << "Init python log for android with result (0 for success):" << result << base::logmess;
}

#endif

ScriptManager::ScriptManager()
{
}

ScriptManager::~ScriptManager()
{
}

void ScriptManager::addPath(const std::string &path)
{
    if (m_sysPaths.find(path) != m_sysPaths.end())
        return;

    auto sysPath = PySys_GetObject("path");
    if (!sysPath)
    {
        base::glog << "Can not append path" << path << "to Python system path. Failed to get 'path'" << base::logwarn;
        return;
    }

    auto pathObj = PyString_FromString(path.c_str());
    if (!pathObj)
        return;

    if (PyList_Append(sysPath, pathObj) < 0)
        base::glog << "Can not append path" << path << "to Python system path." << base::logwarn;
    else
        m_sysPaths.insert(path);

    Py_DECREF(pathObj);
}

std::string ScriptManager::createAndAddFullPath(const char *path)
{
#if defined(__ANDROID__)
    return path;
#elif defined(__WIN32__)
    auto fullp = g_fileSystem->fullPath(path);
    if (fullp.empty())
    {
        return "";
    }

    addPath(resources::FileSystem::getFileDirectory(fullp));

    return fullp;
#endif
}

bool ScriptManager::init()
{
    base::glog << "Starting Python interpreter" << base::logmess;

    // Start Python interpreter.
    try
    {
        Py_OptimizeFlag = 1;
        Py_Initialize();

#if defined(__ANDROID__)
        initPythonAndroidLog();
        PyRun_SimpleString("import android_boot");

        // FIXME:
        // XXX!!!
        addPath("/data/data/com.flamingo.ships3d/files/script");
#endif

        return true;
    }
    catch (...)
    {
        base::glog << "Error starting Python interpreter:" << base::logcritical;
        PyErr_Print();
        return false;
    }
}

void ScriptManager::shutdown()
{
    // Boost::Python claims to not use Py_Finalize. This may be fixed in a future version of boost.python.
    //Py_Finalize();
}

bool ScriptManager::doFile(const char *fileName)
{
    auto fullp = createAndAddFullPath(fileName);
    if (fullp.empty())
    {
        base::glog << "Can not execute Python file" << fileName << ". File doesn't exist" << base::logwarn;
        return false;
    }

#if defined(__ANDROID__)
    // FIXME:
    // XXX!!!
    fullp = std::string("/data/data/com.flamingo.ships3d/files/") + fullp;
#endif

    try
    {
        auto mainModule = boost::python::import("__main__");
        auto moduleNamespace(mainModule.attr("__dict__"));

        boost::python::exec_file(fullp.c_str(), moduleNamespace, moduleNamespace);

        return true;
    }
    catch (...)
    {
        base::glog << "Error executing Python file" << fileName << base::logwarn;
        PyErr_Print();
    }
    return false;
}

bool ScriptManager::doString(const char *str)
{
    try
    {
        auto mainModule = boost::python::import("__main__");
        auto moduleNamespace(mainModule.attr("__dict__"));

        boost::python::exec_statement(str, moduleNamespace, moduleNamespace);

        return true;
    }
    catch (...)
    {
        base::glog << "Error executing Python statement" << base::logwarn;
        PyErr_Print();
    }
    return false;
}

void ScriptManager::printError()
{
    PyErr_Print();
}

shared_ptr<PythonUpdateProxy> ScriptManager::createUpdateProxy(const char *pyFile, const char *fnName)
{
    try
    {
        return shared_ptr<PythonUpdateProxy>(new PythonUpdateProxy(pyFile, fnName, this));
    }
    catch (...)
    {
        base::glog << "Error get update proxy from" << pyFile << fnName << base::logwarn;
        printError();

        return nullptr;
    }
}

shared_ptr<PythonMouseInputProxy> ScriptManager::createMouseInputProxy(const char *pyFile, const char *pyOnMouseButton, const char *pyOnMouseMotion)
{
    try
    {
        auto proxy = shared_ptr<PythonMouseInputProxy>(new PythonMouseInputProxy(this, pyFile));
        proxy->setButtonEventListener(pyOnMouseButton);
        proxy->setMotionEventListener(pyOnMouseMotion);

        return proxy;
    }
    catch (...)
    {
        base::glog << "Error get mouse input proxy from" << pyFile << base::logwarn;
        printError();

        return nullptr;
    }
}

shared_ptr<PythonKeyboardInputProxy> ScriptManager::createKeyboardInputProxy(const char *pyFile, const char *pyOnKeyDown, const char *pyOnKeyUp)
{
    try
    {
        auto proxy = shared_ptr<PythonKeyboardInputProxy>(new PythonKeyboardInputProxy(this, pyFile));
        proxy->setKeyUpEventListener(pyOnKeyUp);
        proxy->setKeyDownEventListener(pyOnKeyDown);

        return proxy;
    }
    catch (...)
    {
        base::glog << "Error get keyboard input proxy from" << pyFile << base::logwarn;
        printError();

        return nullptr;
    }
}

} }