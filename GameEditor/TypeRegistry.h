#pragma once
#include <Jimara/Core/TypeRegistration/TypeRegistration.h>

namespace GameEditor {
	JIMARA_REGISTER_TYPE(GameEditor::GameEditor_TypeRegistry);
#define TypeRegistry_TMP_DLL_EXPORT_MACRO
	/// <summary> Type registry for our game </summary>
	JIMARA_DEFINE_TYPE_REGISTRATION_CLASS(GameEditor_TypeRegistry, TypeRegistry_TMP_DLL_EXPORT_MACRO);
#undef TypeRegistry_TMP_DLL_EXPORT_MACRO
}
