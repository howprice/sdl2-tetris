//--------------------------------------------------------------------------------------------------
/**
    \file    Game.h
**/
//--------------------------------------------------------------------------------------------------

#ifndef GAME_H
#define GAME_H

class Renderer;

struct uint2
{
	unsigned int x;
	unsigned int y;
};

struct int2
{
	int x;
	int y;
};

struct Field
{
	unsigned int width;
	unsigned int height;
	int* staticBlocks;		// -1 = empty
};

struct Tetromino
{
	static const unsigned int kNumBlocks = 4;
	static const unsigned int kNumRotations = 4;
	typedef uint2 BlockCoords[kNumBlocks];
    
	BlockCoords blockCoord[kNumRotations];
	unsigned int rgba;
};

enum TetrominoType
{
	kTetrominoType_I = 0,
 	kTetrominoType_J,
	kTetrominoType_L,
	kTetrominoType_O,
	kTetrominoType_S,
	kTetrominoType_T,
	kTetrominoType_Z,
	kNumTetrominoTypes
};

struct TetrominoInstance
{
	TetrominoType m_tetrominoType;
	int2 m_pos;
	unsigned int m_rotation;
};

struct GameInput
{
	bool bStart;
	bool bMoveLeft;
	bool bMoveRight;
	bool bRotateClockwise;			// TODO: Check going the right way!
	bool bRotateAnticlockwise;		// TODO: Check going the right way!
	bool bHardDrop;
	bool bSoftDrop;
	bool bPause;

#ifdef _DEBUG
	bool bDebugChangeTetromino;
	bool bDebugMoveLeft;
	bool bDebugMoveRight;
	bool bDebugMoveUp;
	bool bDebugMoveDown;
#endif
};

//--------------------------------------------------------------------------------------------------
/**
    \class    Game
**/
//--------------------------------------------------------------------------------------------------

class Game
{
public:

	Game();
	~Game();

	bool			Init();
	void			Shutdown();
	void			Reset();
	void			Update( const GameInput& gameInput, float deltaTimeSeconds );
	void			Draw( Renderer& renderer );

private:

	void			InitPlaying();
	void			UpdatePlaying( const GameInput& gameInput );
	void			DrawPlaying( Renderer& renderer );

	bool			SpawnTetronimo();
	void			AddTetronimoToField( const Field& field, const TetrominoInstance& tetronimoInstance );

	// state
	float m_deltaTimeSeconds;
	Field m_field;
	TetrominoInstance m_activeTetromino;

	int m_framesUntilFall;
	int m_framesPerFallStep;	// this changes when you level-up

	unsigned int m_numUserDropsForThisTetronimo;

	// score
	unsigned int m_numLinesCleared;
	unsigned int m_level;
	unsigned int m_score;
	unsigned int m_hiScore;

	enum GameState
	{
		kGameState_TitleScreen = 0,
		kGameState_Playing,
		kGameState_GameOver,
		kNumGameStates
	};

	GameState m_gameState;
};

#endif // GAME_H
