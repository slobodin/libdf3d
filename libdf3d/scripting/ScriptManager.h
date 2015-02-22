#pragma once

namespace sel { class State; }

namespace df3d { namespace scripting {

class DF3D_DLL ScriptManager : public boost::noncopyable
{
    unique_ptr<sel::State> m_state;

public:
    ScriptManager();
    ~ScriptManager();

    bool doFile(const char *fileName);
    bool doString(const char *str);
    void printError();

    sel::State &getState();
};

} }
