#include <Engine.h>
#include <stdio.h>
#include "irrKlang.h"

#ifdef _WIN32
extern "C"
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

	// http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
//else: Use GPU by default.
#endif

int main()
{
	irrklang::ISoundEngine *engine = irrklang::createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engine

	// play some sound stream, looped

	engine->play2D("res/sounds/ophelia.mp3", true);

	while (getchar() != 'q')
	{
	}
	// irrKlang:
	engine->drop(); // delete engine

	return 0;
}