#include <Engine.h>
#include <cstdio>
#include <iostream>
#include "irrKlang.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef _WIN32
extern "C"
{
	// http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;

	// http://developer.amd.com/community/blog/2015/10/02/amd-enduro-system-for-developers/
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
//else: Use GPU by default.
#endif

int main()
{
	/* FreeType2 font library example */
	FT_Library  library;   /* handle to library     */
	FT_Face     face;      /* handle to face object */

	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		std::cerr << "freetype font library faild to initialize\n";
	}

	error = FT_New_Face(library,
		"res/fonts/DroidSans.ttf",
		0,
		&face);

	if (error == FT_Err_Unknown_File_Format)
	{
		std::cerr << "the font file could be openedand read, but it appears that its font format is unsupported\n";
	}
	else if (error)
	{
		std::cerr << "another error code means that the font file could not be opened or read, or that it is broken\n";
	}


	/* irrKlang audio library example */
	irrklang::ISoundEngine *engine = irrklang::createIrrKlangDevice();

	if (!engine)
		return 0; // error starting up the engine

	// play some sound stream, looped
	engine->play2D("res/sounds/ophelia.mp3", true);

	if (getchar() == 'n')
	{
		engine->play2D("res/sounds/szkic.mp3", false);
	}
	while (getchar() != 'q')
	{
	}
	// irrKlang:
	engine->drop(); // delete engine

	return 0;
}