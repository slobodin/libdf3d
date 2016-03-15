#include "TbManager.h"

#include <tb_core.h>
#include "impl/TBInterface.h"

namespace df3d {

TBManager::TBManager()
{
    tb::tb_core_init(gui_impl::CreateRenderer());

    void register_tbbf_font_renderer();
    register_tbbf_font_renderer();
}

TBManager::~TBManager()
{
    tb::tb_core_shutdown();
}

}
