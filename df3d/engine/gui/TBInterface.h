#pragma once

namespace tb { class TBRenderer; }

namespace df3d {

unique_ptr<tb::TBRenderer> CreateTBRenderer();

}
