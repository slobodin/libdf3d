#pragma once

namespace tb {
class TBRenderer;
class TBSkin;
class TBWidgetsReader;
class TBLanguage;
class TBFontManager;
class TBWidget;
}

namespace df3d {

class DF3D_DLL GuiManager : NonCopyable
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
};

}
