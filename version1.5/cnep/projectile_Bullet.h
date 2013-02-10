/********************************************************************
	created:	2011/07/19
	filename: 	projectile_Bullet.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain projecile for MGD machine gun
*********************************************************************/
#ifndef GUARD_Projectile_BULLET_HEADER_FILE
#define GUARD_Projectile_BULLET_HEADER_FILE

#include "Projectile.h"

class projectile_Bullet: public Projectile
{
	SEGAN_STERILE_CLASS(projectile_Bullet);

public:

	projectile_Bullet(void);
	virtual ~projectile_Bullet(void);

	virtual void AddToScene( void );

	virtual void Update(float elpsTime);

	virtual Projectile* Clone(void);

	void DamageToTarget( Entity* target );

public:

	float3	m_initPos;
};

#endif	//	GUARD_Projectile_BULLET_HEADER_FILE