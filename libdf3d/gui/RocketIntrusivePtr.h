#pragma once

namespace Rocket { namespace Core { class ReferenceCountable; } }

namespace boost
{

void DF3D_DLL intrusive_ptr_add_ref(Rocket::Core::ReferenceCountable *p);
void DF3D_DLL intrusive_ptr_release(Rocket::Core::ReferenceCountable *p);

}
