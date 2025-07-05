#include "__Generated__/TypeRegistry.impl.h"
#include <Jimara-StateMachines/Types.h>
#include <Jimara-GenericInputs/Types.h>

namespace Game {
	static Jimara::Reference<Game_TypeRegistry> registryInstance = nullptr;
	static Jimara::Reference<Jimara::StateMachines_TypeRegistry> stateMachineTypes = nullptr;
	static Jimara::Reference<Jimara::GenericInputs_TypeRegistry> genericInputTypes = nullptr;

	inline static void Game_OnLibraryLoad() {
		registryInstance = Game_TypeRegistry::Instance();
		stateMachineTypes = Jimara::StateMachines_TypeRegistry::Instance();
		genericInputTypes = Jimara::GenericInputs_TypeRegistry::Instance();
	}

	inline static void Game_OnLibraryUnload() {
		registryInstance = nullptr;
		stateMachineTypes = nullptr;
		genericInputTypes = nullptr;
	}
}

extern "C" {
#ifdef _WIN32
#include <windows.h>
	BOOL WINAPI DllMain(_In_ HINSTANCE, _In_ DWORD fdwReason, _In_ LPVOID) {
		if (fdwReason == DLL_PROCESS_ATTACH) Game::Game_OnLibraryLoad();
		else if (fdwReason == DLL_PROCESS_DETACH) Game::Game_OnLibraryUnload();
		return TRUE;
	}
#else
	__attribute__((constructor)) static void DllMain() {
		Game::Game_OnLibraryLoad();
	}
	__attribute__((destructor)) static void OnStaticObjectUnload() {
		Game::Game_OnLibraryUnload();
	}
#endif
}
