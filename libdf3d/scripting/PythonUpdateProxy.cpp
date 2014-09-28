#include "df3d_pch.h"
#include "PythonUpdateProxy.h"

#include "ScriptManager.h"

namespace df3d { namespace scripting {

#if DF3D_USES_PYTHON

struct PythonUpdateProxy::Impl
{
    boost::function<void (float)> updateFn;
};

PythonUpdateProxy::PythonUpdateProxy(const char *pyFile, const char *pyFunction, ScriptManager *sm)
    : m_pImpl(new Impl())
{
    m_pImpl->updateFn = sm->getPythonObject<boost::function<void(float)>>(pyFile, pyFunction);
}

PythonUpdateProxy::~PythonUpdateProxy()
{

}

void PythonUpdateProxy::update(float dt)
{
    try
    {
        if (m_pImpl->updateFn)
            m_pImpl->updateFn(dt);
    }
    catch (...)
    {
        PyErr_Print();
    }
}

#endif

} }