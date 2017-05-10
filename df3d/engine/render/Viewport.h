#pragma once

namespace df3d {

class Viewport
{
    int m_x;
    int m_y;
    int m_w;
    int m_h;

public:
    Viewport(int x = 0, int y = 0, int w = DEFAULT_WINDOW_WIDTH, int h = DEFAULT_WINDOW_HEIGHT);

    void setDimensions(int x, int y, int w, int h);
    int x() const;
    int y() const;
    int width() const;
    int height() const;

    bool operator== (const Viewport &other) const;
    bool operator!= (const Viewport &other) const;
};

}
