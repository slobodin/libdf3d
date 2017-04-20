#pragma once

#include <df3d/lib/NonCopyable.h>
#include <df3d/Common.h>

namespace tb {
class TBRenderer;
class TBSkin;
class TBWidgetsReader;
class TBLanguage;
class TBFontManager;
class TBWidget;
class TBImageManager;
}

namespace df3d {

class GuiManager : NonCopyable
{
    unique_ptr<tb::TBWidget> m_root;
    int m_width, m_height;

    unique_ptr<tb::TBRenderer> m_renderer;

public:
    GuiManager();
    ~GuiManager();

    void initialize(int contextWidth, int contextHeight);
    void shutdown();
    void update();
    void replaceRoot();

    void showDebugger();

    tb::TBWidget* getRoot() { return m_root.get(); }
    tb::TBRenderer* getRenderer();
    tb::TBSkin* getSkin();
    tb::TBWidgetsReader* getWidgetsReader();
    tb::TBLanguage* getLang();
    tb::TBFontManager* getFontManager();
    tb::TBImageManager* getImageManager();
};

}
