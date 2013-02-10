/********************************************************************
	created:	2012/08/07
	filename: 	projectile_ClusterBomb.h
	Author:		Sajad Beigjani
	eMail:		sajad.b@gmail.com
	Site:		www.SeganX.com
	Desc:		This file contain class cluster bomb
*********************************************************************/
#ifndef GUARD_projectile_ClusterBomb_HEADER_FILE
#define GUARD_projectile_ClusterBomb_HEADER_FILE


#include "Projectile.h"

class projectile_ClusterBomb: public Projectile
{
	SEGAN_STERILE_CLASS(projectile_ClusterBomb);

public:

	projectile_ClusterBomb(void);
	virtual ~projectile_ClusterBomb(void);

	virtual void Update(float elpsTime);

	virtual Projectile* Clone(void);

public:
	WCHAR			m_miniBombName[128];	//	name of mini bombs
	Projectile*		m_miniBomb;				//	projectile type as mini bomb

	int				m_state;				//  state of projectile
	float			m_time;					//  time of movement
	float3			m_turnDir;				//	new direction
};

#endif	//	GUARD_projectile_ClusterBomb_HEADER_FILE
