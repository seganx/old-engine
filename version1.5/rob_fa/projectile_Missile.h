/********************************************************************
	created:	2012/11/20
	filename: 	projectile_Missile.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class cluster bomb
*********************************************************************/
#ifndef GUARD_projectile_Missile_HEADER_FILE
#define GUARD_projectile_Missile_HEADER_FILE


#include "Projectile.h"

class projectile_Missile: public Projectile
{
	SEGAN_STERILE_CLASS(projectile_Missile);

public:

	projectile_Missile(void);
	virtual ~projectile_Missile(void);

	virtual void Update(float elpsTime);

	virtual Projectile* Clone(void);

public:
	int				m_state;		//  state of projectile
	float			m_time;			//  time of movement
	float3			m_turnDir;		//	new direction
};

#endif	//	GUARD_projectile_Missile_HEADER_FILE
