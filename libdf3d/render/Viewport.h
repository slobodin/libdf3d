#pragma once

namespace df3d { namespace render {

class DF3D_DLL Viewport
{
    int m_x = 0;
    int m_y = 0;
    int m_w = DEFAULT_WINDOW_WIDTH;
    int m_h = DEFAULT_WINDOW_HEIGHT;

    bool isDirty = false;

public:
    void setDimensions(int x, int y, int w, int h);
    int x() const;
    int y() const;
    int width() const;
    int height() const;
};

} }