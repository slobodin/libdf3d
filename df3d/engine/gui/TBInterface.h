#pragma once

#include <df3d/Common.h>

namespace tb { class TBRenderer; }

namespace df3d {

unique_ptr<tb::TBRenderer> CreateTBRenderer();

}
