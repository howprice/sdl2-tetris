//--------------------------------------------------------------------------------------------------
/**
    \file    Game.cpp
**/
//--------------------------------------------------------------------------------------------------

#include "Game.h"

#include "Debug.h"
#include "Renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static const unsigned int s_kFieldWidth = 10;
static const unsigned int s_kFieldHeight = 20;
static const unsigned int s_kNumHiddenRows = 2;		// TODO: Use this region to spawn tetrominoes out of sight (centred)
static const unsigned int s_initialFramesPerFallStep = 48;
static const int s_deltaFramesPerFallStepPerLevel = 2;

//--------------------------------------------------------------------------------------------------

// From Super Rotation System standard http://tetris.wikia.com/wiki/SRS

static const Tetromino s_tetrominos[kNumTetrominoTypes] =
{
	// I
	{
		0, 1, 1, 1, 2, 1, 3, 1,
		2, 0, 2, 1, 2, 2, 2, 3,
		0, 2, 1, 2, 2, 2, 3, 2,
		1, 0, 1, 1, 1, 2, 1, 3,
		0x00ffffff,
	},
	// J
	{
		0, 0, 0, 1, 1, 1, 2, 1,
		1, 0, 2, 0, 1, 1, 1, 2,
		0, 1, 1, 1, 2, 1, 2, 2,
		1, 0, 1, 1, 0, 2, 1, 2,
		0x0000ffff,
	},
	// L
	{
		2, 0, 0, 1, 1, 1, 2, 1,
		1, 0, 1, 1, 1, 2, 2, 2,
		0, 1, 1, 1, 2, 1, 0, 2,
		0, 0, 1, 0, 1, 1, 1, 2,
		0xffaa00ff,
	},
	// O
	{
		1, 0, 2, 0, 1, 1, 2, 1,
		1, 0, 2, 0, 1, 1, 2, 1,
		1, 0, 2, 0, 1, 1, 2, 1,
		1, 0, 2, 0, 1, 1, 2, 1,
		0xffff00ff
	},
	// S
	{
		1, 0, 2, 0, 0, 1, 1, 1,
		1, 0, 1, 1, 2, 1, 2, 2,
		1, 1, 2, 1, 0, 2, 1, 2,
		0, 0, 0, 1, 1, 1, 1, 2,
		0x00ff00ff,
	},
	// T
	{
		1, 0, 0, 1, 1, 1, 2, 1,
		1, 0, 1, 1, 2, 1, 1, 2,
		0, 1, 1, 1, 2, 1, 1, 2,
		1, 0, 0, 1, 1, 1, 1, 2,
		0x9900ffff,
	},
	// Z
	{
		0, 0, 1, 0, 1, 1, 2, 1,
		2, 0, 1, 1, 2, 1, 1, 2,
		0, 1, 1, 1, 1, 2, 2, 2,
		1, 0, 0, 1, 1, 1, 0, 2,
		0xff0000ff,
	}
};

//--------------------------------------------------------------------------------------------------

static bool IsOverlap( const TetrominoInstance& tetronimoInstance, const Field& field )
{
	const Tetromino& tetronimo = s_tetrominos[tetronimoInstance.m_tetrominoType];
	const Tetromino::BlockCoords& blockCoords = tetronimo.blockCoord[ tetronimoInstance.m_rotation ];
	for( unsigned int i = 0; i < Tetromino::kNumBlocks; ++ i )
	{
		const int x = tetronimoInstance.m_pos.x + blockCoords[i].x;
		const int y = tetronimoInstance.m_pos.y + blockCoords[i].y;

		// Count going outside the field as an overlap
		if( x < 0 || x >= (int)field.width || y < 0 || y >= (int)field.height )
			return true;

		if( field.staticBlocks[x + y * field.width] != -1 )
			return true;
	}

	return false;
}

static void SetBlock( Field& field, unsigned int ix, unsigned int iy, unsigned int val )
{
	HP_ASSERT( ix < field.width );
	HP_ASSERT( iy < field.height );
	field.staticBlocks[iy * field.width + ix] = val;
}

//--------------------------------------------------------------------------------------------------

Game::Game()
: m_deltaTimeSeconds( 0.0f )
, m_framesUntilFall( s_initialFramesPerFallStep )
, m_framesPerFallStep( s_initialFramesPerFallStep )
, m_numUserDropsForThisTetronimo( 0 )
, m_numLinesCleared( 0 )
, m_level( 0 )
, m_score( 0 )
, m_hiScore( 0 )
, m_gameState( kGameState_TitleScreen )
{
	m_field.staticBlocks = nullptr;
}

Game::~Game()
{
}

bool Game::Init()
{
	return true;
}

void Game::Shutdown()
{
	delete [] m_field.staticBlocks;
	m_field.staticBlocks = nullptr;
}

void Game::Reset()
{

}

// return true if there was room to spawn
bool Game::SpawnTetronimo()
{
	m_activeTetromino.m_tetrominoType = (TetrominoType)( rand() % kNumTetrominoTypes );
	m_activeTetromino.m_rotation = 0;
	m_activeTetromino.m_pos.x = (m_field.width - 4) / 2;	// tetronimo block width approx = 4
	m_activeTetromino.m_pos.y = 0;

	if( IsOverlap( m_activeTetromino, m_field ) )
	{
		return false;
	}

	m_framesUntilFall = s_initialFramesPerFallStep;
	m_numUserDropsForThisTetronimo = 0;
	return true;
}

void Game::Update( const GameInput& gameInput, float deltaTimeSeconds )
{
	m_deltaTimeSeconds = deltaTimeSeconds;

	switch( m_gameState )
	{
	case kGameState_TitleScreen:
		if( gameInput.bStart )
		{
			InitPlaying();
			m_gameState = kGameState_Playing;
		}
		break;
	case kGameState_Playing:
		UpdatePlaying( gameInput );
		break;
	case kGameState_GameOver:
		if( gameInput.bStart )
		{
			m_gameState = kGameState_TitleScreen;
		}
		break;
	}
}

void Game::InitPlaying()
{
	m_field.width = s_kFieldWidth;
	m_field.height = s_kFieldHeight;
	delete [] m_field.staticBlocks;
	m_field.staticBlocks = new int[m_field.width * m_field.height];

	for( unsigned int iy = 0; iy < m_field.height; ++iy )
	{
		for( unsigned int ix = 0; ix < m_field.width; ++ix )
		{
			m_field.staticBlocks[iy * m_field.width + ix] = -1;
		}
	}

	srand( (unsigned int)time( NULL ) );

	SpawnTetronimo();

	m_numLinesCleared = 0;
	m_level = 0;
	m_framesPerFallStep = s_initialFramesPerFallStep;
	m_score = 0;
}

void Game::UpdatePlaying( const GameInput& gameInput )
{
#ifdef _DEBUG
	if( gameInput.bDebugChangeTetromino )
	{
		m_activeTetromino.m_tetrominoType = (TetrominoType)( ( (unsigned int)m_activeTetromino.m_tetrominoType + 1 ) % (unsigned int)kNumTetrominoTypes );
	}
	if( gameInput.bDebugMoveLeft )
	{
		--m_activeTetromino.m_pos.x;
	}
	if( gameInput.bDebugMoveRight )
	{
		++m_activeTetromino.m_pos.x;
	}
	if( gameInput.bDebugMoveUp )
	{
		--m_activeTetromino.m_pos.y;
	}
	if( gameInput.bDebugMoveDown )
	{
		++m_activeTetromino.m_pos.y;
	}
#endif

	// move horizontally
	if( gameInput.bMoveLeft )
	{
		// try move
		TetrominoInstance testInstance = m_activeTetromino;
		--testInstance.m_pos.x;
		if( !IsOverlap(testInstance, m_field) )
			m_activeTetromino.m_pos.x = testInstance.m_pos.x;
	}
	if( gameInput.bMoveRight )
	{
		// try move
		TetrominoInstance testInstance = m_activeTetromino;
		++testInstance.m_pos.x;
		if( !IsOverlap( testInstance, m_field ) )
			m_activeTetromino.m_pos.x = testInstance.m_pos.x;
	}


	// rotate
	if( gameInput.bRotateClockwise )
	{
		TetrominoInstance testInstance = m_activeTetromino;
		if( testInstance.m_rotation == 0 )
		{
			testInstance.m_rotation = 3;
		}
		else
		{
			--testInstance.m_rotation;
		}

		// check for overlaps
		if( IsOverlap( testInstance, m_field ) )
		{
			// Simple wall kick, try 1 to the left and 1 to the right
			// TODO: Need special case for the stick, which may need to be bumped by +/-2
			testInstance.m_pos.x = m_activeTetromino.m_pos.x - 1;
			if( !IsOverlap( testInstance, m_field ) )
			{
				m_activeTetromino = testInstance;
			}
			else
			{
				testInstance.m_pos.x = m_activeTetromino.m_pos.x + 1;
				if( !IsOverlap( testInstance, m_field ) )
				{
					m_activeTetromino = testInstance;
				}
			}
		}
		else
		{
			// no overlap
			m_activeTetromino = testInstance;
		}
	}

	if( gameInput.bRotateAnticlockwise )
	{
		m_activeTetromino.m_rotation = ( m_activeTetromino.m_rotation + 1 ) % Tetromino::kNumRotations;
	}

	// fall
	m_framesUntilFall -= 1;
	if( m_framesUntilFall <= 0 )
	{
		m_framesUntilFall = m_framesPerFallStep;

		// try move
		TetrominoInstance testInstance = m_activeTetromino;
		testInstance.m_pos.y += 1;
		if( IsOverlap(testInstance, m_field) )
		{
			AddTetronimoToField( m_field, m_activeTetromino );
			if( !SpawnTetronimo() )
				m_gameState = kGameState_GameOver;
		}
		else
		{
			m_activeTetromino.m_pos.y = testInstance.m_pos.y;
		}
	}

	// soft drop
	if( gameInput.bSoftDrop )
	{
		// try move
		TetrominoInstance testInstance = m_activeTetromino;
		++testInstance.m_pos.y;
		if( !IsOverlap( testInstance, m_field ) )
		{
			m_activeTetromino.m_pos.y = testInstance.m_pos.y;
			++m_numUserDropsForThisTetronimo;
		}
	}

	// hard drop
	if( gameInput.bHardDrop )
	{
		TetrominoInstance testInstance = m_activeTetromino;
		while( !IsOverlap( testInstance, m_field ) )
		{
			++testInstance.m_pos.y;
			++m_numUserDropsForThisTetronimo;
		}
		--testInstance.m_pos.y;	// back up one
		--m_numUserDropsForThisTetronimo;
		AddTetronimoToField( m_field, testInstance );
		if( !SpawnTetronimo() )
			m_gameState = kGameState_GameOver;
	}

}

void Game::AddTetronimoToField( const Field& field, const TetrominoInstance& tetronimoInstance )
{
	const Tetromino& tetronimo = s_tetrominos[tetronimoInstance.m_tetrominoType];
	const Tetromino::BlockCoords& blockCoords = tetronimo.blockCoord[tetronimoInstance.m_rotation];
	for( unsigned int i = 0; i < Tetromino::kNumBlocks; ++i )
	{
		const int x = tetronimoInstance.m_pos.x + blockCoords[i].x;
		const int y = tetronimoInstance.m_pos.y + blockCoords[i].y;

		// Count going outside the field as an overlap
		HP_ASSERT( x >= 0 && x < (int)field.width && y >= 0 || y >= (int)field.height )

			field.staticBlocks[x + y * field.width] = (unsigned int)tetronimoInstance.m_tetrominoType;
	}

	// clear any full rows
	unsigned int numLinesCleared = 0;
	for( unsigned int y = 0; y < field.height; ++y )
	{
		bool bRowFull = true;
		for( unsigned int x = 0; x < field.width; ++x )
		{
			if( field.staticBlocks[x + y * field.width] == -1 )
			{
				bRowFull = false;
				break;
			}
		}

		if( bRowFull )
		{
			++numLinesCleared;

			// traverse backward back up to the top copying down the data
			for( unsigned int yy = y; yy > 0; --yy )
			{
				for( unsigned int x = 0; x < field.width; ++x )
				{
					field.staticBlocks[x + yy * field.width] = field.staticBlocks[x + ( yy - 1 ) * field.width];
				}
			}
		}
	}

	unsigned int previousLevel = m_numLinesCleared / 10;
	m_numLinesCleared += numLinesCleared;
	m_level = m_numLinesCleared / 10;

	if( m_level != previousLevel )
	{
		// update speed
		m_framesPerFallStep -= s_deltaFramesPerFallStepPerLevel;
		if( m_framesPerFallStep < 1 )
		{
			m_framesPerFallStep = 1;
		}
	}

	// score
	if( numLinesCleared > 0 )
	{
		unsigned int multiplier = 0;
		switch( numLinesCleared )
		{
		case 1:
			multiplier = 40;
			break;
		case 2:
			multiplier = 100;
			break;
		case 3:
			multiplier = 300;
			break;
		case 4:
			multiplier = 1200;
		}

		unsigned int score = multiplier * ( previousLevel + 1 );
		score += m_numUserDropsForThisTetronimo;
		m_score += score;
		if( m_score > m_hiScore )
			m_hiScore = m_score;
	}

}


void Game::Draw( Renderer& renderer )
{
	switch( m_gameState )
	{
	case kGameState_TitleScreen:
		// TODO: GetTextRect
		renderer.DrawText( "Press space to start", renderer.GetLogicalWidth()/2 - 100, renderer.GetLogicalHeight()/2, 0xffffffff );
		break;
	case kGameState_Playing:
		DrawPlaying( renderer );
		break;
	case kGameState_GameOver:
		DrawPlaying( renderer );
		renderer.DrawText( "GAME OVER", renderer.GetLogicalWidth() / 2 - 100, renderer.GetLogicalHeight() / 2, 0xffffffff );
		break;
	}

#ifdef _DEBUG
	float fps = 1.0f / m_deltaTimeSeconds;
	char text[128];
	sprintf_s( text, "FPS: %.1f", fps );
	renderer.DrawText( text, 0, 0, 0x8080ffff );
#endif
}

void Game::DrawPlaying( Renderer& renderer )
{
	static unsigned int blockSizePixels = 32;

	// Draw field

	unsigned int fieldWidthPixels = m_field.width * blockSizePixels;
	unsigned int fieldHeightPixels = m_field.height * blockSizePixels;

	unsigned int fieldOffsetPixelsX = 0;
	if( renderer.GetLogicalWidth() > fieldWidthPixels )
	{
		fieldOffsetPixelsX = ( renderer.GetLogicalWidth() - fieldWidthPixels ) / 2;
	}

	unsigned int fieldOffsetPixelsY = 0;
	if( renderer.GetLogicalHeight() > fieldHeightPixels )
	{
		fieldOffsetPixelsY = ( renderer.GetLogicalHeight() - fieldHeightPixels ) / 2;
	}

	for( unsigned int iy = 0; iy < m_field.height; ++iy )
	{
		const unsigned int y = fieldOffsetPixelsY + iy * blockSizePixels;

		for( unsigned int ix = 0; ix < m_field.width; ++ix )
		{
			const unsigned int x = fieldOffsetPixelsX + ix * blockSizePixels;

			const int blockState = m_field.staticBlocks[iy * m_field.width + ix];
			unsigned int blockRgba = 0x202020ff;
			if( blockState != -1 )
			{
				HP_ASSERT( blockState < kNumTetrominoTypes );
				blockRgba = s_tetrominos[blockState].rgba;
			}

			renderer.DrawSolidRect( x, y, blockSizePixels, blockSizePixels, blockRgba );
			renderer.DrawRect( x, y, blockSizePixels, blockSizePixels, 0x404040ff );
		}
	}


	// draw active tetromino
	for( unsigned int i = 0; i < 4; ++i )
	{
		const Tetromino& tetromino = s_tetrominos[m_activeTetromino.m_tetrominoType];
		const Tetromino::BlockCoords& blockCoords = tetromino.blockCoord[m_activeTetromino.m_rotation];
		unsigned int tetrominoRgba = tetromino.rgba;
		const unsigned int x = fieldOffsetPixelsX + ( m_activeTetromino.m_pos.x + blockCoords[i].x ) * blockSizePixels;
		const unsigned int y = fieldOffsetPixelsY + ( m_activeTetromino.m_pos.y + blockCoords[i].y ) * blockSizePixels;
		renderer.DrawSolidRect( x, y, blockSizePixels, blockSizePixels, tetrominoRgba );
	}

	char text[128];
	sprintf_s( text, "Lines: %u", m_numLinesCleared );
	renderer.DrawText( text, 0, 100, 0xffffffff );
	sprintf_s( text, "Level: %u", m_level );
	renderer.DrawText( text, 0, 140, 0xffffffff );
	sprintf_s( text, "Score: %u", m_score );
	renderer.DrawText( text, 0, 180, 0xffffffff );
	sprintf_s( text, "High score: %u", m_hiScore );
	renderer.DrawText( text, 0, 220, 0xffffffff );

#ifdef _DEBUG
	sprintf_s( text, "Frames per fall: %u", m_framesPerFallStep );
	renderer.DrawText( text, 0, 400, 0x404040ff );
#endif
}
