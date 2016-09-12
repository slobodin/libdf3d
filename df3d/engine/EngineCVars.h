#pragma once

#include "ConfigVariable.h"

namespace df3d {

class DF3D_DLL EngineCVars
{
public:
    static ConfigVariableBool bulletDebugDraw;
    static ConfigVariableInt preferredFPS;
    static ConfigVariableBool objIndexize;
};

}
