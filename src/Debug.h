//--------------------------------------------------------------------------------------------------
/**
    \file   Debug.h
**/
//--------------------------------------------------------------------------------------------------

#ifndef DEBUG_H
#define DEBUG_H

#define HP_UNUSED(X)	(void)X

#if defined _MSC_VER
#define HP_BREAK __debugbreak();
#elif defined __arm__
#define HP_BREAK __builtin_trap();
#elif defined __GNUC__
#define HP_BREAK __asm__ ("int $3");
#else
#error Unsupported compiler
#endif

#define HP_ASSERT( expression ) \
	if( !(expression) ) { \
	fprintf( stderr, "ASSERT:\t%s\nFile:\t%s\nLine:\t%d\n\n", \
	#expression, __FILE__, __LINE__ ); \
	HP_BREAK  \
	}

#define HP_ASSERT_MSG( expression, message ) \
	if( !(expression) ) { \
	fprintf( stderr, "ASSERT:\t%s\nFile:\t%s\nLine:\t%d\nMessage:\t%s\n", \
		#expression, __FILE__, __LINE__, #message ); \
		HP_BREAK  \
	}

#define HP_FATAL_ERROR( message ) \
	fprintf( stderr, "FATAL ERROR: %s\nFile:\t%s\nLine:\t%d\n\n", #message, __FILE__, __LINE__ ); \
	HP_BREAK

#endif // DEBUG_H
