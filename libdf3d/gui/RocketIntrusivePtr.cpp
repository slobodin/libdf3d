#include "df3d_pch.h"
#include "RocketIntrusivePtr.h"

#include <Rocket/Core.h>

namespace boost
{

void DF3D_DLL intrusive_ptr_add_ref(Rocket::Core::ReferenceCountable *p)
{
    p->AddReference();
}

void DF3D_DLL intrusive_ptr_release(Rocket::Core::ReferenceCountable *p)
{
    p->RemoveReference();
}

}