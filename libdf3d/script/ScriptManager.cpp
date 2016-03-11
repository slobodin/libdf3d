#include "ScriptManager.h"

#include <sqstdaux.h>
#include <sqstdio.h>
#include <sqstdmath.h>
#include <sqstdstring.h>
#include <sqstdblob.h>
#include <cstdarg>
#include <cstdio>
#include <sqrat/sqrat.h>
#include <libdf3d/base/EngineController.h>
#include <libdf3d/io/FileSystem.h>
#include <libdf3d/io/FileDataSource.h>
#include <libdf3d/utils/Utils.h>

namespace df3d {

static void printfunc(HSQUIRRELVM v, const SQChar *s, ...)
{
    va_list arglist;
    va_start(arglist, s);

    char buffer[1024] = { 0 };
    scvsprintf(buffer, 1024, s, arglist);

    glog.printWithoutFormat(buffer, logscript);

    va_end(arglist);
}

ScriptManager::ScriptManager()
{

}

ScriptManager::~ScriptManager()
{

}

void ScriptManager::initialize()
{
    glog << "Starting Squirrel" << logmess;

    m_squirrel = sq_open(1024);

    sq_pushroottable(m_squirrel); // push the root table where the std function will be registered

    if (!SQ_SUCCEEDED(sqstd_register_mathlib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel math library");

    if (!SQ_SUCCEEDED(sqstd_register_stringlib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel string library");

    if (!SQ_SUCCEEDED(sqstd_register_bloblib(m_squirrel)))
        throw std::runtime_error("Failed to register squirrel blob library");

    sq_pop(m_squirrel, 1); // pops the root table

    sqstd_seterrorhandlers(m_squirrel);
    sq_setprintfunc(m_squirrel, printfunc, printfunc);

    // Bind df3d code.
    // FIXME: figure out how to share (static member functions in sqrat)
    //script_impl::bindDf3d(m_squirrel);
}

void ScriptManager::shutdown()
{
    sq_close(m_squirrel);
    m_squirrel = nullptr;
}

bool ScriptManager::doFile(const std::string &fileName)
{
    if (auto file = svc().fileSystem().openFile(fileName))
    {
        // FIXME: store execution result too.
        if (utils::contains_key(m_executedFiles, file->getPath()))
            return true;

        std::string buffer(file->getSizeInBytes(), 0);
        file->getRaw(&buffer[0], buffer.size());

        glog << "Executing" << fileName << logmess;
        m_executedFiles.insert(file->getPath());

        file.reset();

        return doString(buffer.c_str());
    }
    else
    {
        glog << "Failed to execute" << fileName << ". File doesn't exist" << logwarn;
        return false;
    }
}

bool ScriptManager::doString(const SQChar *str)
{
    Sqrat::Script squirrelScript(m_squirrel);

    Sqrat::string errMsg;
    if (!squirrelScript.CompileString(str, errMsg))
    {
        glog << "Failed to compile squirrel script:" << errMsg.c_str() << logwarn;
        DEBUG_BREAK();

        return false;
    }

    if (!squirrelScript.Run(errMsg))
    {
        glog << "Failed to run squirrel script:" << errMsg.c_str() << logwarn;
        DEBUG_BREAK();

        return false;
    }

    return true;
}

void ScriptManager::gc()
{
    if (!SQ_SUCCEEDED(sq_collectgarbage(m_squirrel)))
        glog << "Squirrel: Failed to collect garbage" << logscript;
}

}
