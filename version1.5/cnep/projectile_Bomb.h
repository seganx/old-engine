/********************************************************************
	created:	2011/12/12
	filename: 	Projectile_BOMB.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain a projectile class for BOMB
*********************************************************************/
#ifndef GUARD_Projectile_BOMB_HEADER_FILE
#define GUARD_Projectile_BOMB_HEADER_FILE

#include "Projectile.h"

class Projectile_BOMB: public Projectile
{
	SEGAN_STERILE_CLASS(Projectile_BOMB);

public:

	Projectile_BOMB(void);
	virtual ~Projectile_BOMB(void);

	virtual void AddToScene( void );

	virtual void Update(float elpsTime);

	virtual Projectile* Clone(void);

};

#endif	//	GUARD_Projectile_BOMB_HEADER_FILE
