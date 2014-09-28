#pragma once

FWD_MODULE_CLASS(base, Controller)

namespace df3d { namespace scripting {

#if DF3D_USES_PYTHON

class PythonUpdateProxy;
class PythonMouseInputProxy;
class PythonKeyboardInputProxy;

class DF3D_DLL ScriptManager : boost::noncopyable
{
    friend class base::Controller;

    ScriptManager();
    ~ScriptManager();

    std::set<std::string> m_sysPaths;
    void addPath(const std::string &path);
    std::string createAndAddFullPath(const char *path);

    // These are only for controller.
    bool init();
    void shutdown();

public:
#if defined(DF3D_LIBRARY)
    template<typename T>
    T getPythonObject(const char *pyFile, const char *pyObjName);
#endif

    bool doFile(const char *fileName);
    bool doString(const char *str);
    void printError();

    shared_ptr<PythonUpdateProxy> createUpdateProxy(const char *pyFile, const char *fnName);
    shared_ptr<PythonMouseInputProxy> createMouseInputProxy(const char *pyFile, const char *pyOnMouseButton, const char *pyOnMouseMotion);
    shared_ptr<PythonKeyboardInputProxy> createKeyboardInputProxy(const char *pyFile, const char *pyOnKeyDown, const char *pyOnKeyUp);
};

#else

    class DF3D_DLL ScriptManager : public boost::noncopyable
    {
        friend class base::Controller;

        bool init() { }
        void shutdown() { }

        bool doFile(const char *fileName) { return false; }
        bool doString(const char *str) { return false; }
        void printError() { }
    };


#endif

} }

#if defined(DF3D_LIBRARY)
#include "ScriptManager.inl"
#endif
