#include "__Generated__/TypeRegistry.impl.h"
#include <Jimara-StateMachines-Editor/Types.h>

namespace GameEditor {
	static Jimara::Reference<GameEditor_TypeRegistry> registryInstance = nullptr;
	static Jimara::Reference<Jimara::StateMachinesEditor_TypeRegistry> stateMachineEditorTypes = nullptr;

	inline static void GameEditor_OnLibraryLoad() {
		registryInstance = GameEditor_TypeRegistry::Instance();
		stateMachineEditorTypes = Jimara::StateMachinesEditor_TypeRegistry::Instance();
	}

	inline static void GameEditor_OnLibraryUnload() {
		registryInstance = nullptr;
		stateMachineEditorTypes = nullptr;
	}
}

extern "C" {
#ifdef _WIN32
#include <windows.h>
	BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD fdwReason, _In_ LPVOID) {
		if (fdwReason == DLL_PROCESS_ATTACH) GameEditor::GameEditor_OnLibraryLoad();
		else if (fdwReason == DLL_PROCESS_DETACH) GameEditor::GameEditor_OnLibraryUnload();
		return TRUE;
	}
#else
	__attribute__((constructor)) static void DllMain() {
		GameEditor::GameEditor_OnLibraryLoad();
	}
	__attribute__((destructor)) static void OnStaticObjectUnload() {
		GameEditor::GameEditor_OnLibraryUnload();
	}
#endif
}
