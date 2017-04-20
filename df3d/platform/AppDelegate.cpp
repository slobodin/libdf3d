#include "AppDelegate.h"

#include <cassert>

namespace df3d {

static AppDelegate *g_appDelegate = nullptr;

void AppDelegate::setInstance(AppDelegate *app)
{
    assert(g_appDelegate == nullptr);
    g_appDelegate = app;
}

AppDelegate* AppDelegate::getInstance()
{
    return g_appDelegate;
}

}
