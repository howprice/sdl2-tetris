#include <SDL.h>
#include <stdio.h>

static void print_SDL_version( const char* preamble, const SDL_version& v )
{
	printf( "%s %u.%u.%u\n", preamble, v.major, v.minor, v.patch );
}

int main( int /*argc*/, char** /*argv*/ )
{
	if( SDL_Init( SDL_INIT_EVERYTHING ) != 0)
	{
		fprintf( stderr, "SDL failed to initialise: %s\n",SDL_GetError() );
		return 1;
	}

	printf( "SDL initialised\n" );

	SDL_version compiledVersion;
	SDL_version linkedVersion;
	SDL_VERSION( &compiledVersion );
	SDL_GetVersion( &linkedVersion );
	print_SDL_version( "Compiled against SDL version", compiledVersion );
	print_SDL_version( "Linking against SDL version", linkedVersion );

	const char* title = "SDL Window";
	const int windowWidth = 640;
	const int windowHeight = 480;
	SDL_Window*	pWindow = SDL_CreateWindow( title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN );
	if( !pWindow )
	{
		fprintf( stderr, "Failed to create SDL window: %s\n", SDL_GetError() );
		return 1;
	}

	SDL_Surface* pWindowSurface = SDL_GetWindowSurface( pWindow );
	SDL_FillRect( pWindowSurface, NULL, SDL_MapRGB( pWindowSurface->format, 0xff, 0x00, 0x00 ) );
	SDL_UpdateWindowSurface( pWindow );
	SDL_Delay(2000);

	SDL_DestroyWindow( pWindow );
	pWindow = nullptr;
	SDL_Quit();

	return 0;
}
