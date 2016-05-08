//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2013 :                                                    //
//  - Julien Fryer - julienfryer@gmail.com				                        //
//  - Thibault Lescoat - info-tibo@orange.fr                                    //
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

#ifndef H_SPK_DESCRIPTIONDEFINES
#define H_SPK_DESCRIPTIONDEFINES

#include "SPK_Meta.h"

/**
* @brief Starts the definition of a type description.
*/
#define spark_description( _classname_ , _parentobject_ )									\
	public:																					\
		template<typename, bool> friend struct SPK::meta::Cloner;							\
		virtual SPK::Ref<SPK::SPKObject> clone() const override								\
		{																					\
			return SPK::meta::Cloner<_classname_ >::clone(this);							\
		}																					\

#endif
