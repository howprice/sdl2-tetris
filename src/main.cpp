//--------------------------------------------------------------------------------------------------
/**
    \file    main.cpp
**/
//--------------------------------------------------------------------------------------------------

#include "App.h"

#include <SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char** argv )
{
	bool bFullScreen = false;
	unsigned int displayWidth = 1280;
	unsigned int displayHeight = 720;
	for( int i = 1; i < argc; ++i )
	{
		if( strcmp( argv[i], "--fullscreen" ) == 0 )
		{
			bFullScreen = true;
		}
		else if( strcmp( argv[i], "--width" ) == 0)
		{
			SDL_assert( argc > i ); // make sure we have another argument
			displayWidth = (unsigned int)atoi( argv[++i] );
		}
		else if(strcmp( argv[i], "--height" ) == 0)
		{
			SDL_assert( argc > i ); // make sure we have another argument
			displayHeight = (unsigned int)atoi( argv[++i] );
		}
	}

	App app;
	if( !app.Init( bFullScreen, displayWidth, displayHeight ) )
	{
		printf( "ERROR - App failed to initialise\n" );
		app.ShutDown();
		return 1;
	}

	app.Run();

	app.ShutDown();

	return 0;
}
