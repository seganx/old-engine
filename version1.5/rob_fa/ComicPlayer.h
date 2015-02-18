/********************************************************************
	created:	2012/09/23
	filename: 	Mechanic_Cinematic.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a simple mechanic to play cinematic in game
*********************************************************************/
#ifndef GUARD_ComicPlayer_HEADER_FILE
#define GUARD_ComicPlayer_HEADER_FILE

#include "ImportEngine.h"

struct ComicSound
{
	float	time;		//	time of playing sound
	uint	index;		//	index of playing sound
	wchar	name[32];	//	name of the sound member
};

struct ComicParticle
{
	float	time;		//	time of playing particle
	wchar	name[32];	//	name of the particle member
};

class ComicPlayer
{
public:
	ComicPlayer( void );
	~ComicPlayer( void );

	bool Load( const wchar* fileName );
	void Update( float elpstime );
	void Draw( void );
	bool Playing( void );

public:
	float m_time;
	float m_maxTime;
	sx::core::Node* m_node;
	sx::core::Node* m_camera;
	Array<ComicSound> m_sounds;
	Array<ComicParticle> m_particles;
};

#endif // GUARD_ComicPlayer_HEADER_FILE
