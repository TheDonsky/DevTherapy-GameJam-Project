#include <Jimara/Environment/StandaloneRunner.h>
#include <Windows.h>


int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	Jimara::StandaloneRunner::Args args = {};
	args.assetDirectory = Jimara::OS::Path(std::filesystem::is_directory("Assets") ? "Assets" : "../../../Assets/");
	return static_cast<int>(Jimara::StandaloneRunner::RunGame(args));
}
