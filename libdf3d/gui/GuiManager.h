#pragma once

#include "impl/RocketIntrusivePtr.h"
#include <input/InputEvents.h>
#include <Rocket/Core.h>

namespace Rocket { namespace Core { class Context; } }

namespace df3d {

namespace gui_impl {

class FileInterface;
class SystemInterface;
class RenderInterface;

}

class DF3D_DLL GuiManager : utils::NonCopyable
{
    unique_ptr<gui_impl::FileInterface> m_fileInterface;
    unique_ptr<gui_impl::SystemInterface> m_systemInterface;
    unique_ptr<gui_impl::RenderInterface> m_renderInterface;

    Rocket::Core::Context *m_rocketContext = nullptr;

public:
    GuiManager(int contextWidth, int contextHeight);
    ~GuiManager();

    RocketDocument loadDocument(const std::string &name);

    void showDebugger(bool show);
    bool isDebuggerVisible() const;

    Rocket::Core::Context* getContext() { return m_rocketContext; }

    template<typename T>
    T* createDocument(const std::string &id)
    {
        return dynamic_cast<T*>(m_rocketContext->CreateDocument(id.c_str()));
    }
};

}
