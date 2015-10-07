//--------------------------------------------------------------------------------------------------
/**
    \file    Renderer.cpp
**/
//--------------------------------------------------------------------------------------------------

#include "Renderer.h"

#include "Debug.h"

#include <stdio.h>

#include <SDL.h>

//--------------------------------------------------------------------------------------------------
// Helpers

static SDL_Color MakeSDL_Colour( uint32_t rgba )
{
	SDL_Color color;
	color.r = (unsigned char)( ( rgba >> 24 ) & 0xff );
	color.g = (unsigned char)( ( rgba >> 16 ) & 0xff );
	color.b = (unsigned char)( ( rgba >> 8 ) & 0xff );
	color.a = (unsigned char)( ( rgba >> 0 ) & 0xff );		// alpha
	return color;
}

//--------------------------------------------------------------------------------------------------

Renderer::Renderer( SDL_Window& window, unsigned int logicalWidth, unsigned int logicalHeight )
  : m_logicalWidth(0)
  , m_logicalHeight(0)
  , m_pSdlRenderer(nullptr)
  , m_pFont(nullptr)
{
	Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_pSdlRenderer = SDL_CreateRenderer( &window, -1, rendererFlags );
	if(!m_pSdlRenderer)
	{
		fprintf( stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError() );
		HP_FATAL_ERROR( "Failed to create SDL renderer" );
	}

	// n.b. The display size may not equal the logical size
 	int displayWidth, displayHeight;
 	SDL_GetWindowSize( &window, &displayWidth, &displayHeight );
	printf( "Display size = (%d, %d)\n", displayWidth, displayHeight );
	printf( "Renderer logical size = (%u, %u)\n", logicalWidth, logicalHeight );
	if( displayWidth != (int)logicalWidth || displayHeight != (int)logicalHeight )
	{
		printf( "Logical size != display size (%u, %u) vs (%u, %u). Scaling will be applied\n", logicalWidth, logicalHeight, displayWidth, displayHeight );
	}
	const float displayAspect = (float)displayWidth / (float)displayHeight;
	const float logicalAspect = (float)logicalWidth / (float)logicalHeight;
	if( logicalAspect != displayAspect)
	{
		printf( "Logical aspect != display aspect. Letterboxing will be applied\n" );
	}

	m_logicalWidth = logicalWidth;
	m_logicalHeight = logicalHeight;
	SDL_RenderSetLogicalSize( m_pSdlRenderer, logicalWidth, logicalHeight );

	SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "linear" );  // make the scaled rendering look smoother

	int defaultFontSize = 32;
	m_pFont = TTF_OpenFont( "clacon.ttf", defaultFontSize );
	if( !m_pFont )
	{
		fprintf( stderr, "TTF_OpenFont failed: %s\n", TTF_GetError() );
		HP_FATAL_ERROR( "Failed to open font" )
	}
}

Renderer::~Renderer()
{
	TTF_CloseFont( m_pFont );
	SDL_DestroyRenderer( m_pSdlRenderer );
}

void Renderer::Clear()
{
	SDL_SetRenderDrawColor( m_pSdlRenderer, 0, 0, 0, 255 );
	SDL_RenderClear( m_pSdlRenderer );
}

void Renderer::Present()
{
	SDL_RenderPresent( m_pSdlRenderer );
}

void Renderer::DrawRect( int x, int y, int w, int h, uint32_t rgba /*= 0xffffffff */ )
{
	SDL_Color color = MakeSDL_Colour( rgba );
	SDL_SetRenderDrawColor( m_pSdlRenderer, color.r, color.g, color.b, color.a );

	SDL_Rect rect = {x, y, w, h};
	SDL_RenderDrawRect( m_pSdlRenderer, &rect );
}

void Renderer::DrawSolidRect( int x, int y, int w, int h, uint32_t rgba /*= 0xffffffff */ )
{
	SDL_Color color = MakeSDL_Colour( rgba );
	SDL_SetRenderDrawColor( m_pSdlRenderer, color.r, color.g, color.b, color.a );

	SDL_Rect rect = { x, y, w, h };
	SDL_RenderFillRect( m_pSdlRenderer, &rect );
}

void Renderer::DrawText( const char* text, int x, int y, uint32_t rgba /*= 0xffffffff */ )
{
	SDL_assert( text );

	SDL_Color color = MakeSDL_Colour( rgba );

	SDL_Surface* pSurface = TTF_RenderText_Blended( m_pFont, text, color );
	SDL_Texture* pTexture = SDL_CreateTextureFromSurface( m_pSdlRenderer, pSurface );
	int width, height;
	SDL_QueryTexture(pTexture, NULL, NULL, &width, &height);
	SDL_Rect dstRect = { x, y, width, height };
	SDL_RenderCopy( m_pSdlRenderer, pTexture, nullptr, &dstRect );
	SDL_DestroyTexture( pTexture );
	SDL_FreeSurface( pSurface );
}
