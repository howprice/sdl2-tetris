//--------------------------------------------------------------------------------------------------
/**
    \file    App.h
**/
//--------------------------------------------------------------------------------------------------

#ifndef APP_H
#define APP_H

// SDL forward
struct SDL_Window;

class Game;
class Renderer;

class App
{
public:

	bool	Init( bool bFullScreen, unsigned int logicalWidth, unsigned int logicalHeight );
	void	ShutDown();
	void	Run();

private:

	SDL_Window*			m_pWindow			= nullptr;
	Renderer*			m_pRenderer			= nullptr;
	Game*				m_pGame				= nullptr;
};

#endif // APP_H
