#include "Viewport.h"

namespace df3d {

Viewport::Viewport(int x, int y, int w, int h)
    : m_x(x),
    m_y(y),
    m_w(w),
    m_h(h)
{

}

void Viewport::setDimensions(int x, int y, int w, int h)
{
    if (x < 0 || y < 0 || x >= w || y >= h)
    {
        DFLOG_WARN("Trying to set invalid viewport dimensions");
        return;
    }

    m_x = x;
    m_y = y;
    m_w = w;
    m_h = h;
}

int Viewport::x() const
{
    return m_x;
}

int Viewport::y() const
{
    return m_y;
}

int Viewport::width() const
{
    return m_w;
}

int Viewport::height() const
{
    return m_h;
}

bool Viewport::operator== (const Viewport &other) const
{
    return m_x == other.m_x && m_y == other.m_y && m_w == other.m_w && m_h == other.m_h;
}

bool Viewport::operator!= (const Viewport &other) const
{
    return !(*this == other);
}

}
