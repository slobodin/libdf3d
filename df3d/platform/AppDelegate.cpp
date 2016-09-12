#include "AppDelegate.h"

namespace df3d {

static AppDelegate *g_appDelegate = nullptr;

AppDelegate::AppDelegate()
{
    assert(g_appDelegate == nullptr);
    g_appDelegate = this;
}

AppDelegate* AppDelegate::getInstance()
{
    return g_appDelegate;
}

}
