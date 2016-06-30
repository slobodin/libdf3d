#pragma once

namespace tb { class TBRenderer; }

namespace df3d { namespace gui_impl {

unique_ptr<tb::TBRenderer> CreateRenderer();

} }
